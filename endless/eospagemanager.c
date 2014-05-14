/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eospagemanager.h"

#include <gtk/gtk.h>

#include <string.h>

/**
 * SECTION:page-manager
 * @short_description: Controlling the flow of your application
 * @title: Page Manager
 *
 * Your users experience your application as a series of
 * <emphasis>pages</emphasis> &mdash; screens that present a small amount
 * of information or show one feature before moving on to the next page
 * or a previous page.
 *
 * The <emphasis>page manager</emphasis> controls how these pages relate to
 * each other.
 * There are several different page managers available, each representing a
 * different user interaction.
 * The default page manager, described in this section of the manual, lets you
 * add any number of pages and switch between them however you like, but there
 * are also other, more specialized ones:
 * for example, the #EosSplashPageManager displays a splash screen and
 * later turns control over to a different page or page manager when you signal
 * it to;
 * and the #EosTabbedPageManager creates a tabbed interface in your window, much
 * like the one in your browser.
 *
 * Each window has a page manager; one is created by default when you create
 * the window, but you can replace it by a different one.
 * You can also nest page managers, one inside the other, in order to create
 * more complex application flows.
 *
 * A page can be any widget, most likely a container widget with other widgets
 * inside it.
 * To add a page to a page manager, call
 * |[
 * gtk_container_add (GTK_CONTAINER (page_manager), page);
 * ]|
 * If the added page is the only page, then the page manager will display it
 * immediately.
 * If the page manager was already displaying another page, then adding a new
 * page will not change which page is displayed.
 *
 * To get information about how to display the pages, for example the background
 * image to use, the page manager reads the page's <emphasis>child
 * properties</emphasis>.
 * These are like regular properties, but instead of modifying the page, they
 * modify the relationship between the page and the page manager that contains
 * it.
 * Most pages have at least a name and a background image as child properties.
 * You can add a page with child properties as follows:
 * |[
 * gtk_container_add_with_properties (GTK_CONTAINER (page_manager), page,
 *                                    "name", "front-page",
 *                                    "background_uri", "image.jpg",
 *                                    NULL);
 * ]|
 * In Javascript, this has been simplified to use JSON:
 * |[
 * page_manager.add(page, {
 *     name: 'front-page',
 *     background_uri: 'image.jpg'
 * });
 * ]|
 * To remove a page, use gtk_container_remove() or
 * eos_page_manager_remove_page_by_name().
 * If the removed page was the only page, then the page manager will display
 * nothing.
 * If there are multiple pages still in the page manager, you should never
 * remove the visible-child. Always set a new visible child before removing the
 * current one. A critical warning will be emitted if you remove the visible-
 * page when there are still other pages in the page manager.
 *
 * In general, it is convenient to refer to a page by its name when dealing with
 * the page manager, so you should make a point of giving all your pages names.
 */

#define DEFAULT_BACKGROUND_SIZE "100% 100%"
#define DEFAULT_BACKGROUND_POSITION "0% 0%"

typedef struct {
  GtkWidget *page;
  GtkWidget *left_topbar_widget;
  GtkWidget *center_topbar_widget;
  gchar *background_uri;
  gchar *background_size;
  gchar *background_position;
  gboolean background_repeats;
} EosPageManagerPageInfo;

typedef struct {
  GList *page_info; /* GList<EosPageManagerPageInfo> */
  GHashTable *pages_by_widget; /* GHashTable<GtkWidget *, EosPageManagerPageInfo *> */
} EosPageManagerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosPageManager, eos_page_manager, GTK_TYPE_STACK)

enum
{
  CHILD_PROP_0,
  CHILD_PROP_LEFT_TOPBAR_WIDGET,
  CHILD_PROP_CENTER_TOPBAR_WIDGET,
  CHILD_PROP_BACKGROUND_URI,
  CHILD_PROP_BACKGROUND_SIZE,
  CHILD_PROP_BACKGROUND_POSITION,
  CHILD_PROP_BACKGROUND_REPEATS,
  NCHILDPROPS
};

static GParamSpec *eos_page_manager_child_props[NCHILDPROPS] = { NULL, };

static void
page_info_free (EosPageManagerPageInfo *info)
{
  g_free (info->background_uri);
  g_free (info->background_size);
  g_free (info->background_position);
  g_slice_free (EosPageManagerPageInfo, info);
}

static void
top_bars_unref (EosPageManagerPageInfo *info)
{
  g_clear_object (&info->left_topbar_widget);
  g_clear_object (&info->center_topbar_widget);
}

/*
 * find_page_info_by_widget:
 * @self: the page manager
 * @page: the page to look for
 *
 * Searches for the page info corresponding to the child @page.
 *
 * Returns: the #EosPageManagerPageInfo for @page, or %NULL if @page is not a
 * child of @self.
 */
static EosPageManagerPageInfo *
find_page_info_by_widget (EosPageManager *self,
                          GtkWidget      *page)
{
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);
  return g_hash_table_lookup (priv->pages_by_widget, page);
}

/* Convenience function, since this warning occurs at several places */
static void
warn_page_widget_not_found (EosPageManager *self,
                            GtkWidget      *page)
{
  g_critical ("Page at %p (type %s) is not a child of EosPageManager %p",
              page,
              g_type_name (G_OBJECT_TYPE (page)),
              self);
}

/* Invariants: number of pages in list and number of pages in pages_by_widget
hash table must be equal; and number of pages in pages_by_name hash table must
be equal or less. This check is expensive, should only be enabled for debugging.
*/
static void
assert_internal_state (EosPageManager *self)
{
#ifdef DEBUG
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);
  guint list_length = g_list_length (priv->page_info);
  g_assert_cmpuint (list_length,
                    ==,
                    g_hash_table_size (priv->pages_by_widget));
#endif
}

static void
eos_page_manager_dispose (GObject *object)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);
  g_list_foreach (priv->page_info, (GFunc)top_bars_unref, NULL);

  G_OBJECT_CLASS (eos_page_manager_parent_class)->dispose (object);
}

static void
eos_page_manager_finalize (GObject *object)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);

  G_OBJECT_CLASS (eos_page_manager_parent_class)->finalize (object);

  g_list_foreach (priv->page_info, (GFunc)page_info_free, NULL);
  g_hash_table_destroy(priv->pages_by_widget);
}

static void
eos_page_manager_add (GtkContainer *container,
                      GtkWidget    *new_page)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);

  EosPageManagerPageInfo *info = g_slice_new0 (EosPageManagerPageInfo);
  info->background_size = g_strdup (DEFAULT_BACKGROUND_SIZE);
  info->background_position = g_strdup (DEFAULT_BACKGROUND_POSITION);
  info->background_repeats = TRUE;
  info->page = new_page;
  priv->page_info = g_list_prepend (priv->page_info, info);
  g_hash_table_insert (priv->pages_by_widget, new_page, info);

  GTK_CONTAINER_CLASS (eos_page_manager_parent_class)->add (container, new_page);
  assert_internal_state (self);
}

static void
eos_page_manager_remove (GtkContainer *container,
                         GtkWidget    *page)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);

  GTK_CONTAINER_CLASS (eos_page_manager_parent_class)->remove (container, page);
  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return;
    }
  priv->page_info = g_list_remove (priv->page_info, info);
  g_hash_table_remove (priv->pages_by_widget, page);

  page_info_free (info);

  assert_internal_state (self);
}

static void
eos_page_manager_get_child_property (GtkContainer *container,
                                     GtkWidget    *child,
                                     guint         property_id,
                                     GValue       *value,
                                     GParamSpec   *pspec)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);

  switch (property_id)
    {
    case CHILD_PROP_BACKGROUND_URI:
      g_value_set_string (value,
                          eos_page_manager_get_page_background_uri (self,
                                                                    child));
      break;

    case CHILD_PROP_BACKGROUND_SIZE:
      g_value_set_string (value,
                          eos_page_manager_get_page_background_size (self,
                                                                     child));
      break;

    case CHILD_PROP_BACKGROUND_POSITION:
      g_value_set_string (value,
                          eos_page_manager_get_page_background_position (self,
                                                                         child));
      break;

    case CHILD_PROP_BACKGROUND_REPEATS:
      g_value_set_boolean (value,
                          eos_page_manager_get_page_background_repeats (self,
                                                                        child));
      break;

    case CHILD_PROP_LEFT_TOPBAR_WIDGET:
      g_value_set_object (value,
                          eos_page_manager_get_page_left_topbar_widget (self,
                                                                        child));
      break;

    case CHILD_PROP_CENTER_TOPBAR_WIDGET:
      g_value_set_object (value,
                          eos_page_manager_get_page_center_topbar_widget (self,
                                                                          child));
      break;

    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container,
                                                    property_id, pspec);
    }
}

static void
eos_page_manager_set_child_property (GtkContainer *container,
                                     GtkWidget    *child,
                                     guint         property_id,
                                     const GValue *value,
                                     GParamSpec   *pspec)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);

  switch (property_id)
    {
    case CHILD_PROP_BACKGROUND_URI:
      eos_page_manager_set_page_background_uri (self, child,
                                                g_value_get_string (value));
      break;

    case CHILD_PROP_BACKGROUND_SIZE:
      eos_page_manager_set_page_background_size (self, child,
                                                 g_value_get_string (value));
      break;

    case CHILD_PROP_BACKGROUND_POSITION:
      eos_page_manager_set_page_background_position (self, child,
                                                     g_value_get_string (value));
      break;

    case CHILD_PROP_BACKGROUND_REPEATS:
      eos_page_manager_set_page_background_repeats (self, child,
                                                    g_value_get_boolean (value));
      break;

    case CHILD_PROP_LEFT_TOPBAR_WIDGET:
      eos_page_manager_set_page_left_topbar_widget (self, child,
                                                    g_value_get_object (value));
      break;

    case CHILD_PROP_CENTER_TOPBAR_WIDGET:
      eos_page_manager_set_page_center_topbar_widget (self, child,
                                                    g_value_get_object (value));
      break;

    default:
      GTK_CONTAINER_WARN_INVALID_CHILD_PROPERTY_ID (container,
                                                    property_id, pspec);
    }
}

static void
eos_page_manager_class_init (EosPageManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->dispose = eos_page_manager_dispose;
  object_class->finalize = eos_page_manager_finalize;

  container_class->add = eos_page_manager_add;
  container_class->remove = eos_page_manager_remove;
  container_class->get_child_property = eos_page_manager_get_child_property;
  container_class->set_child_property = eos_page_manager_set_child_property;

  /**
   * EosPageManager:left-topbar-widget:
   *
   * The left topbar widget belonging to this page, to be displayed on the
   * left portion of the top bar when the page is displaying. Setting this to
   * %NULL indicates that there should be no left topbar widget.
   */
  eos_page_manager_child_props[CHILD_PROP_LEFT_TOPBAR_WIDGET] =
    g_param_spec_object ("left-topbar-widget", "Left topbar widget",
                         "Left topbar widget displayed left of the topbar",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:center-topbar-widget:
   *
   * The center topbar widget belonging to this page, to be displayed on the
   * middle portion of the top bar when the page is displaying. Setting this to
   * %NULL indicates that there should be no center topbar widget.
   */
  eos_page_manager_child_props[CHILD_PROP_CENTER_TOPBAR_WIDGET] =
    g_param_spec_object ("center-topbar-widget", "Center topbar widget",
                         "Center topbar widget displayed in the middle of the topbar",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:background-uri:
   *
   * The URI for the image file for the background of this page. Setting this to
   * %NULL indicates that the window's default background should be used.
   */
  eos_page_manager_child_props[CHILD_PROP_BACKGROUND_URI] =
    g_param_spec_string ("background-uri", "Background URI",
                         "URI for background of the page",
                         NULL,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:background-size:
   *
   * The size of the page background, given as a css snippet string. This
   * string can be set to any valid css value for the background-size
   * property. See https://developer.mozilla.org/en-US/docs/Web/CSS/background-size
   */
  eos_page_manager_child_props[CHILD_PROP_BACKGROUND_SIZE] =
    g_param_spec_string ("background-size", "Background Size",
                         "Size of background of the page",
                         DEFAULT_BACKGROUND_SIZE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:background-position:
   *
   * The position of the page background, given as a css snippet string. This
   * string can be set to any valid css position value. See
   * https://developer.mozilla.org/en-US/docs/Web/CSS/position_value
   */
  eos_page_manager_child_props[CHILD_PROP_BACKGROUND_POSITION] =
    g_param_spec_string ("background-position", "Background Position",
                         "Position of background of the page",
                         DEFAULT_BACKGROUND_POSITION,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:background-repeats:
   *
   * This child property controls whether or not the background of the page
   * will tile when drawn. Defaults to TRUE.
   */
  eos_page_manager_child_props[CHILD_PROP_BACKGROUND_REPEATS] =
    g_param_spec_boolean ("background-repeats", "Background Repeats",
                         "If page background repeats",
                         TRUE,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /* Install child properties all at once, because there is no
  gtk_container_class_install_child_properties() function */
  int count;
  for (count = CHILD_PROP_0 + 1; count < NCHILDPROPS; count++)
    gtk_container_class_install_child_property (container_class, count,
                                                eos_page_manager_child_props[count]);
}

static void
eos_page_manager_init (EosPageManager *self)
{
  EosPageManagerPrivate *priv = eos_page_manager_get_instance_private (self);
  priv->pages_by_widget = g_hash_table_new (g_direct_hash,
                                            g_direct_equal);
}

/* Public API */

/**
 * eos_page_manager_new:
 *
 * Creates a new default page manager.
 *
 * Returns: the new page manager.
 */
GtkWidget *
eos_page_manager_new (void)
{
  return g_object_new (EOS_TYPE_PAGE_MANAGER, NULL);
}

/**
 * eos_page_manager_get_page_left_topbar_widget:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Retrieves @page's left topbar widget, if it has one.
 * See #EosPageManager:left-topbar-widget for more information.
 * 
 * Returns: (transfer none): the left topbar #GtkWidget of @page, or %NULL if there is none.
 */
GtkWidget *
eos_page_manager_get_page_left_topbar_widget (EosPageManager *self,
                                              GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->left_topbar_widget;
}

/**
 * eos_page_manager_set_page_left_topbar_widget:
 * @self: the page manager
 * @page: the page
 * @left_topbar_widget: (allow-none): left topbar widget for @page
 * 
 * Sets the left topbar widget to be displayed for this @page. 
 * See #EosPageManager:left-topbar-widget for more information.
 */
void
eos_page_manager_set_page_left_topbar_widget (EosPageManager *self,
                                              GtkWidget      *page,
                                              GtkWidget      *left_topbar_widget)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));
  g_return_if_fail (left_topbar_widget == NULL ||
                    GTK_IS_WIDGET (left_topbar_widget));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (info->left_topbar_widget == left_topbar_widget)
    return;

  if (info->left_topbar_widget)
    g_object_unref (info->left_topbar_widget);

  g_object_ref (left_topbar_widget);
  info->left_topbar_widget = left_topbar_widget;

  gtk_container_child_notify (GTK_CONTAINER (self), page,
                              "left-topbar-widget");
}

/**
 * eos_page_manager_get_page_center_topbar_widget:
 * @self: the page manager
 * @page: the page to be queried
 * 
 * Retrieves @page's center topbar widget, if it has one.
 * See #EosPageManager:center-topbar-widget for more information.
 * 
 * Returns: (transfer none): the center topbar #GtkWidget of @page, or %NULL if there is none.
 */
 GtkWidget *
 eos_page_manager_get_page_center_topbar_widget (EosPageManager *self,
                                                 GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->center_topbar_widget;
}

/**
 * eos_page_manager_set_page_center_topbar_widget:
 * @self: the page manager
 * @page: the page
 * @center_topbar_widget: (allow-none): center topbar widget for @page
 * 
 * Sets the center topbar widget to be displayed for this @page.
 * See #EosPageManager:center-topbar-widget for more information.
 */
void
eos_page_manager_set_page_center_topbar_widget (EosPageManager *self,
                                                GtkWidget      *page,
                                                GtkWidget      *center_topbar_widget)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));
  g_return_if_fail (center_topbar_widget == NULL ||
                    GTK_IS_WIDGET (center_topbar_widget));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (info->center_topbar_widget == center_topbar_widget)
    return;

  if (info->center_topbar_widget)
    g_object_unref (info->center_topbar_widget);

  g_object_ref (center_topbar_widget);
  info->center_topbar_widget = center_topbar_widget;

  gtk_container_child_notify (GTK_CONTAINER (self), page,
                              "center-topbar-widget");
}

/**
 * eos_page_manager_get_page_background_uri:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets the URI for the background image of @page, which must previously have
 * been added to the page manager.
 * See #EosPageManager:background-uri for more information.
 *
 * Returns: the background of @page, or the %NULL if @page does not have a
 * background.
 */
const gchar *
eos_page_manager_get_page_background_uri (EosPageManager *self,
                                          GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->background_uri;
}

/**
 * eos_page_manager_set_page_background_uri:
 * @self: the page manager
 * @page: the page to be modified
 * @background: (allow-none): the URI for the background image of this page.
 *
 * Changes the background of @page, which must previously have been added to the
 * page manager.
 * Setting %NULL removes the background, using the window's default background.
 * See #EosPageManager:background-uri for more information.
 */
void
eos_page_manager_set_page_background_uri (EosPageManager *self,
                                          GtkWidget      *page,
                                          const gchar    *background)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (g_strcmp0 (info->background_uri, background) == 0)
    return;

  info->background_uri = g_strdup (background);
  gtk_container_child_notify (GTK_CONTAINER (self), page, "background-uri");
}

/**
 * eos_page_manager_get_page_background_size:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets the size of the background image of @page, which must previously have
 * been added to the page manager.
 * See #EosPageManager:background-size for more information.
 *
 * Returns: the size of @page, as an EosSizePercentage.
 */
const gchar *
eos_page_manager_get_page_background_size (EosPageManager *self,
                                          GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->background_size;
}

/**
 * eos_page_manager_set_page_background_size:
 * @self: the page manager
 * @page: the page to be modified
 * @size: the desired size of the background image of this page.
 *
 * Changes the size of the background of @page, which must previously have
 * been added to the page manager. See #EosPageManager:background-size for
 * more information.
 */
void
eos_page_manager_set_page_background_size (EosPageManager *self,
                                          GtkWidget      *page,
                                          const gchar    *size)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (g_strcmp0 (info->background_size, size) == 0)
    return;

  info->background_size = g_strdup (size);
  gtk_container_child_notify (GTK_CONTAINER (self), page, "background-size");
}


/**
 * eos_page_manager_get_page_background_position:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets the position of the background image of @page, which must previously have
 * been added to the page manager.
 * See #EosPageManager:background-position for more information.
 *
 * Returns: the position of @page, as an EosSizePercentage.
 */
const gchar *
eos_page_manager_get_page_background_position (EosPageManager *self,
                                              GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->background_position;
}

/**
 * eos_page_manager_set_page_background_position:
 * @self: the page manager
 * @page: the page to be modified
 * @position: the desired position of the background image of this page.
 *
 * Changes the position of the background of @page, which must previously have
 * been added to the page manager. See #EosPageManager:background-position for
 * more information.
 */
void
eos_page_manager_set_page_background_position (EosPageManager *self,
                                              GtkWidget      *page,
                                              const gchar    *position)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (g_strcmp0 (info->background_position, position) == 0)
    return;

  info->background_position = g_strdup (position);
  gtk_container_child_notify (GTK_CONTAINER (self), page, "background-position");
}


/**
 * eos_page_manager_get_page_background_repeats:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets whether or not the background image of @page will tile when drawn.
 * @page must previously have been added to the page manager. See
 * #EosPageManager:background-repeats for more information.
 *
 * Returns: True if the background of @page will repeat.
 */
gboolean
eos_page_manager_get_page_background_repeats (EosPageManager *self,
                                              GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), FALSE);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), FALSE);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, FALSE);

  return info->background_repeats;
}

/**
 * eos_page_manager_set_page_background_repeats:
 * @self: the page manager
 * @page: the page to be modified
 * @repeats: True if the background of @page will repeat.
 *
 * Sets whether or not the background image of @page will tile when drawn.
 * @page must previously have been added to the page manager. See
 * #EosPageManager:background-repeats for more information.
 */
void
eos_page_manager_set_page_background_repeats (EosPageManager *self,
                                              GtkWidget      *page,
                                              gboolean        repeats)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  info->background_repeats = repeats;
  gtk_container_child_notify (GTK_CONTAINER (self), page, "background-repeats");
}

/**
 * eos_page_manager_remove_page_by_name:
 * @self: the page manager
 * @name: the name of the page to remove
 *
 * Removes the page called @name from the page manager.
 * If that page was the only page, then the page manager will display nothing.
 * If that page was currently displaying but was not the only page, then the
 * page manager will display another page; which page is undefined.
 *
 * To remove a page without looking it up by name, use gtk_container_remove().
 */
void
eos_page_manager_remove_page_by_name (EosPageManager *self,
                                      const gchar    *name)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (name != NULL);
  GtkContainer *container = GTK_CONTAINER (self);

  GList *l;
  for (l = gtk_container_get_children (container); l != NULL; l = l->next)
    {
      GtkWidget *child = l->data;
      gchar *child_name = NULL;
      gtk_container_child_get (container, child, "name", &child_name, NULL);
      if (child_name != NULL && g_strcmp0 (child_name, name) == 0)
          gtk_container_remove (container, child);
      g_free (child_name);
    }
}
