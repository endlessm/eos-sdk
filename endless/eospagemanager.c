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
 * for example, the #EosSplashScreenPageManager displays a splash screen and
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
 *                                    "background", "image.jpg",
 *                                    NULL);
 * ]|
 * In Javascript, this has been simplified to use JSON:
 * |[
 * page_manager.add(page, {
 *     name: 'front-page',
 *     background: 'image.jpg'
 * });
 * ]|
 * To remove a page, use gtk_container_remove() or
 * eos_page_manager_remove_page_by_name().
 * If the removed page was the only page, then the page manager will display
 * nothing.
 * If that page was currently displaying but was not the only page, then the
 * page manager will display another page; which page is undefined.
 *
 * <warning>
 *   <para>Removing pages with gtk_container_remove() is currently broken due to
 *   a bug in GTK. Use eos_page_manager_remove_page_by_name() for the time
 *   being.</para>
 * </warning>
 *
 * In general, it is convenient to refer to a page by its name when dealing with
 * the page manager, so you should make a point of giving all your pages names.
 */

G_DEFINE_TYPE (EosPageManager, eos_page_manager, GTK_TYPE_CONTAINER)

#define PAGE_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_PAGE_MANAGER, EosPageManagerPrivate))

typedef struct _EosPageManagerPageInfo EosPageManagerPageInfo;
struct _EosPageManagerPageInfo
{
  GtkWidget *page;
  gchar *name;
};

struct _EosPageManagerPrivate
{
  GtkWidget *stack;
  GList *page_info; /* GList<EosPageManagerPageInfo> */
  EosPageManagerPageInfo *visible_page_info;
};

enum
{
  PROP_0,
  PROP_VISIBLE_PAGE,
  PROP_VISIBLE_PAGE_NAME,
  NPROPS
};

enum
{
  CHILD_PROP_0,
  CHILD_PROP_NAME,
  NCHILDPROPS
};

static GParamSpec *eos_page_manager_props[NPROPS] = { NULL, };
static GParamSpec *eos_page_manager_child_props[NCHILDPROPS] = { NULL, };

static void
page_info_free (EosPageManagerPageInfo *info)
{
  g_free (info->name);
  g_slice_free (EosPageManagerPageInfo, info);
}

/* Helper function for find_page_info_by_widget() */
static gint
page_info_widget_compare (const EosPageManagerPageInfo *info,
                          const GtkWidget *page)
{
  return (info->page != page); /* not orderable */
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
  GList *result = g_list_find_custom (self->priv->page_info, page,
                                      (GCompareFunc)page_info_widget_compare);
  if (result == NULL)
    return NULL;
  return result->data;
}

/* Helper function for find_page_info_by_name() */
static gint
page_info_name_compare (const EosPageManagerPageInfo *info,
                        const gchar *name)
{
  /* g_strcmp0() handles NULL */
  return g_strcmp0(info->name, name);
}

/*
 * find_page_info_by_name:
 * @self: the page manager
 * @name: the name to look for
 *
 * Searches for the page info corresponding to the child with name @name.
 *
 * Returns: the #EosPageManagerPageInfo for @name, or %NULL if @name is not the
 * name of a child of @self.
 */
static EosPageManagerPageInfo *
find_page_info_by_name (EosPageManager *self,
                        const gchar    *name)
{
  GList *result = g_list_find_custom (self->priv->page_info, name,
                                      (GCompareFunc)page_info_name_compare);
  if (result == NULL)
    return NULL;
  return result->data;
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

/* Convenience function, since this warning occurs at several places */
static void
warn_page_name_not_found (EosPageManager *self,
                          const gchar    *name)
{
  g_critical ("EosPageManager %p has no page named %s",
              self,
              name);
}

static void
set_visible_page_from_info (EosPageManager         *self,
                            EosPageManagerPageInfo *info)
{
  /* FIXME when porting to GtkStack */
  GtkNotebook *stack_notebook = GTK_NOTEBOOK (self->priv->stack);
  int page_pos = gtk_notebook_page_num (stack_notebook, info->page);
  /* Invariant: if info is not NULL, then page must be in stack */
  g_assert (page_pos != -1);
  gtk_notebook_set_current_page (stack_notebook, page_pos);

  self->priv->visible_page_info = info;

  GObject *self_object = G_OBJECT (self);
  g_object_notify(self_object, "visible-page");
  g_object_notify(self_object, "visible-page-name");
}

static void
eos_page_manager_get_property (GObject    *object,
                               guint       property_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);

  switch (property_id)
    {
    case PROP_VISIBLE_PAGE:
      g_value_set_object (value, eos_page_manager_get_visible_page (self));
      break;

    case PROP_VISIBLE_PAGE_NAME:
      g_value_set_string (value, eos_page_manager_get_visible_page_name (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_page_manager_set_property (GObject      *object,
                               guint         property_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);

  switch (property_id)
    {
    case PROP_VISIBLE_PAGE:
      eos_page_manager_set_visible_page (self, g_value_get_object (value));
      break;

    case PROP_VISIBLE_PAGE_NAME:
      eos_page_manager_set_visible_page_name (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_page_manager_finalize (GObject *object)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);

  g_list_foreach (self->priv->page_info, (GFunc)page_info_free, NULL);

  G_OBJECT_CLASS (eos_page_manager_parent_class)->finalize (object);
}

static GtkSizeRequestMode
eos_page_manager_get_request_mode (GtkWidget *widget)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  return gtk_widget_get_request_mode (self->priv->stack);
}

static void
eos_page_manager_get_preferred_height (GtkWidget       *widget,
                                       gint            *minimum,
                                       gint            *natural)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  gtk_widget_get_preferred_height (self->priv->stack, minimum, natural);
}

static void
eos_page_manager_get_preferred_width_for_height (GtkWidget       *widget,
                                                 gint             height,
                                                 gint            *minimum,
                                                 gint            *natural)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  gtk_widget_get_preferred_height_for_width (self->priv->stack, height,
                                             minimum, natural);
}

static void
eos_page_manager_get_preferred_width (GtkWidget       *widget,
                                      gint            *minimum,
                                      gint            *natural)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  gtk_widget_get_preferred_width (self->priv->stack, minimum, natural);
}

static void
eos_page_manager_get_preferred_height_for_width (GtkWidget       *widget,
                                                 gint             width,
                                                 gint            *minimum,
                                                 gint            *natural)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  gtk_widget_get_preferred_height_for_width (self->priv->stack, width,
                                             minimum, natural);
}

static void
eos_page_manager_size_allocate (GtkWidget *widget,
                                GtkAllocation *allocation)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  gtk_widget_set_allocation (widget, allocation);
  gtk_widget_size_allocate (self->priv->stack, allocation);
}

static void
eos_page_manager_show_all (GtkWidget *widget)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  GTK_WIDGET_CLASS (eos_page_manager_parent_class)->show (widget);
  if (self->priv->stack != NULL)
    gtk_widget_show_all (self->priv->stack);
}

static void
eos_page_manager_map (GtkWidget *widget)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  if (self->priv->stack != NULL && gtk_widget_get_visible (self->priv->stack))
    gtk_widget_map (self->priv->stack);
  GTK_WIDGET_CLASS (eos_page_manager_parent_class)->map (widget);
}

static void
eos_page_manager_unmap (GtkWidget *widget)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  if (self->priv->stack != NULL)
    gtk_widget_unmap (self->priv->stack);
  GTK_WIDGET_CLASS (eos_page_manager_parent_class)->unmap (widget);
}

static gboolean
eos_page_manager_draw (GtkWidget *widget,
                       cairo_t   *cr)
{
  EosPageManager *self = EOS_PAGE_MANAGER (widget);

  if (self->priv->stack != NULL)
    gtk_widget_draw (self->priv->stack, cr);

  return FALSE;
}

static void
eos_page_manager_add (GtkContainer *container,
                      GtkWidget    *new_page)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);

  gtk_container_add (GTK_CONTAINER (self->priv->stack), new_page);
  EosPageManagerPageInfo *info = g_slice_new0 (EosPageManagerPageInfo);
  info->page = new_page;
  self->priv->page_info = g_list_prepend (self->priv->page_info, info);

  /* If there were no pages yet, then this one must become the visible one */
  if (self->priv->visible_page_info == NULL)
    self->priv->visible_page_info = info;
}

static void
eos_page_manager_remove (GtkContainer *container,
                         GtkWidget    *page)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);

  gtk_container_remove (GTK_CONTAINER (self->priv->stack), page);
  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return;
    }
  self->priv->page_info = g_list_remove (self->priv->page_info, info);

  /* If this was the only page */
  if (self->priv->visible_page_info == info)
    self->priv->visible_page_info = NULL;

  page_info_free (info);
}

static void
eos_page_manager_forall (GtkContainer *container,
                         gboolean      include_internals,
                         GtkCallback   callback,
                         gpointer      callback_data)
{
  EosPageManager *self = EOS_PAGE_MANAGER (container);

  if (self->priv->stack == NULL)
    return;

  GtkContainerClass *stack_class = GTK_CONTAINER_GET_CLASS (self->priv->stack);
  stack_class->forall (GTK_CONTAINER (self->priv->stack),
                       include_internals,
                       callback,
                       callback_data);
  if (include_internals)
    callback (self->priv->stack, callback_data);
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
    case CHILD_PROP_NAME:
      g_value_set_string (value, eos_page_manager_get_page_name (self, child));
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
    case CHILD_PROP_NAME:
      eos_page_manager_set_page_name (self, child, g_value_get_string (value));
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
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosPageManagerPrivate));

  object_class->get_property = eos_page_manager_get_property;
  object_class->set_property = eos_page_manager_set_property;
  object_class->finalize = eos_page_manager_finalize;

  /* Pass all size requesting and allocation on to the stack */
  widget_class->get_request_mode = eos_page_manager_get_request_mode;
  widget_class->get_preferred_height = eos_page_manager_get_preferred_height;
  widget_class->get_preferred_height_for_width =
    eos_page_manager_get_preferred_height_for_width;
  widget_class->get_preferred_width = eos_page_manager_get_preferred_width;
  widget_class->get_preferred_width_for_height =
    eos_page_manager_get_preferred_width_for_height;
  widget_class->size_allocate = eos_page_manager_size_allocate;
  widget_class->show_all = eos_page_manager_show_all;
  widget_class->map = eos_page_manager_map;
  widget_class->unmap = eos_page_manager_unmap;
  widget_class->draw = eos_page_manager_draw;

  container_class->add = eos_page_manager_add;
  container_class->remove = eos_page_manager_remove;
  container_class->forall = eos_page_manager_forall;
  container_class->get_child_property = eos_page_manager_get_child_property;
  container_class->set_child_property = eos_page_manager_set_child_property;

  /**
   * EosPageManager:visible-page:
   *
   * A reference to the page widget that is currently being displayed by the
   * page manager.
   * If the page manager has no pages, then this is %NULL.
   */
  eos_page_manager_props[PROP_VISIBLE_PAGE] =
    g_param_spec_object ("visible-page", "Visible page",
                         "Page widget currently displaying in the page manager",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);
  /**
   * EosPageManager:visible-page-name:
   *
   * The name of the page that is currently being displayed by the page manager.
   * If the page manager has no pages, then this is %NULL.
   * However, if there is a page currently being displayed but it has no name,
   * then this is the empty string (<code>""</code>).
   */
  eos_page_manager_props[PROP_VISIBLE_PAGE_NAME] =
    g_param_spec_string ("visible-page-name", "Visible page name",
                         "Name of page currently displaying in the page "
                         "manager",
                         "",
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS,
                                     eos_page_manager_props);

  /**
   * EosPageManager:name:
   *
   * The name of this page. Make sure to choose a unique name.
   */
  eos_page_manager_child_props[CHILD_PROP_NAME] =
    g_param_spec_string ("name", "Name", "Unique ID for the page",
                         NULL,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  gtk_container_class_install_child_property (container_class, CHILD_PROP_NAME,
                                              eos_page_manager_child_props[CHILD_PROP_NAME]);
}

static void
eos_page_manager_init (EosPageManager *self)
{
  GtkWidget *self_widget = GTK_WIDGET (self);
  self->priv = PAGE_MANAGER_PRIVATE (self);

  gtk_widget_set_has_window (self_widget, FALSE);

  /* TODO replace with GtkStack */
  self->priv->stack = gtk_notebook_new ();
  g_object_set (self->priv->stack,
                "show-tabs", FALSE,
                "show-border", FALSE,
                NULL);
  gtk_widget_set_parent (self->priv->stack, self_widget);
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
 * eos_page_manager_get_visible_page:
 * @self: the page manager
 *
 * Gets the page widget that @self is currently displaying.
 * See #EosPageManager:visible-page for more information.
 *
 * Returns: (transfer none): the page #GtkWidget, or %NULL if @self does not
 * have any pages.
 */
GtkWidget *
eos_page_manager_get_visible_page (EosPageManager *self)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), NULL);

  if(self->priv->visible_page_info == NULL)
    return NULL;

  return self->priv->visible_page_info->page;
}

/**
 * eos_page_manager_set_visible_page:
 * @self: the page manager
 * @page: the page to switch to
 *
 * Switches the page manager @self to display @page.
 * The @page widget must previously have been added to the page manager.
 * See #EosPageManager:visible-page for more information.
 */
void
eos_page_manager_set_visible_page (EosPageManager *self,
                                   GtkWidget      *page)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return;
    }

  set_visible_page_from_info (self, info);
}

/**
 * eos_page_manager_get_visible_page_name:
 * @self: the page manager
 *
 * Gets the name of the page widget that @self is currently displaying.
 * See #EosPageManager:visible-page for more information.
 *
 * Returns: the name of the page, or %NULL if @self does not have any pages,
 * or the empty string if the page does not have a name.
 */
const gchar *
eos_page_manager_get_visible_page_name (EosPageManager *self)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), NULL);

  if(self->priv->visible_page_info == NULL)
    return NULL;

  return self->priv->visible_page_info->name;
}

/**
 * eos_page_manager_set_visible_page_name:
 * @self: the page manager
 * @page_name: the name of the page to switch to
 *
 * Switches the page manager @self to display the page called @page_name.
 * This page must previously have been added to the page manager.
 * See #EosPageManager:visible-page for more information.
 */
void
eos_page_manager_set_visible_page_name (EosPageManager *self,
                                        const gchar    *page_name)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));

  EosPageManagerPageInfo *info = find_page_info_by_name (self, page_name);
  if (info == NULL)
    {
      warn_page_name_not_found (self, page_name);
      return;
    }

  set_visible_page_from_info (self, info);
}

/**
 * eos_page_manager_get_page_name:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets the name of @page, which must previously have been added to the
 * page manager.
 * See #EosPageManager:name for more information.
 *
 * Returns: the name of @page, or the empty string if @page does not have a
 * name.
 */
const gchar *
eos_page_manager_get_page_name (EosPageManager *self,
                                GtkWidget      *page)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return NULL;
    }

  if (info->name == NULL)
    return "";

  return info->name;
}

/**
 * eos_page_manager_set_page_name:
 * @self: the page manager
 * @page: the page to be renamed
 * @name: the new name for @page
 *
 * Changes the name of @page, which must previously have been added to the
 * page manager.
 * See #EosPageManager:name for more information.
 */
void
eos_page_manager_set_page_name (EosPageManager *self,
                                GtkWidget      *page,
                                const gchar    *name)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (GTK_IS_WIDGET (page));

  /* Two pages with the same name are not allowed */
  EosPageManagerPageInfo *info = find_page_info_by_name (self, name);
  if (info != NULL && info->page != page)
    {
      g_critical ("Not setting page name to \"%s\", because page manager "
                  "already contains a page by that name",
                  name);
      return;
    }

  info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return;
    }

  if (g_strcmp0(info->name, name) == 0)
    return;

  g_free (info->name);
  info->name = g_strdup (name);

  gtk_container_child_notify (GTK_CONTAINER (self), page, "name");
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

  EosPageManagerPageInfo *info = find_page_info_by_name (self, name);
  if (info == NULL)
    {
      warn_page_name_not_found (self, name);
      return;
    }

  /* FIXME: Can't use gtk_container_remove() directly because that asserts
  gtk_widget_get_parent(child) == self || GTK_IS_ASSISTANT(self)
  See https://bugzilla.gnome.org/show_bug.cgi?id=699756 [endlessm/eos-sdk#67] */
  g_signal_emit_by_name (self, "remove", info->page);
}