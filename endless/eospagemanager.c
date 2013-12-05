/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eospagemanager.h"
#include "eospagemanager-private.h"

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
 * remove the visible-page. Always set a new visible page before removing the
 * current one. A critical warning will be emitted if you remove the visible-
 * page when there are still other pages in the page manager.
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

#define DEFAULT_BACKGROUND_SIZE "100% 100%"
#define DEFAULT_BACKGROUND_POSITION "0% 0%"

G_DEFINE_TYPE (EosPageManager, eos_page_manager, GTK_TYPE_CONTAINER)

#define PAGE_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_PAGE_MANAGER, EosPageManagerPrivate))

typedef struct _EosPageManagerPageInfo EosPageManagerPageInfo;
struct _EosPageManagerPageInfo
{
  GtkWidget *page;
  gchar *name;
  gboolean fake_page_actions_visible;
  GtkWidget *custom_toolbox_widget;
  GtkWidget *left_topbar_widget;
  GtkWidget *center_topbar_widget;
  gchar *background_uri;
  gchar *background_size;
  gchar *background_position;
  gboolean background_repeats;
};

struct _EosPageManagerPrivate
{
  GtkWidget *stack;
  GList *page_info; /* GList<EosPageManagerPageInfo> */
  GHashTable *pages_by_name; /* GHashTable<gchar *, EosPageManagerPageInfo *> */
  GHashTable *pages_by_widget; /* GHashTable<GtkWidget *, EosPageManagerPageInfo *> */
  EosPageManagerPageInfo *visible_page_info;
  EosPageManagerTransitionType transition_type;
};

GType
eos_page_manager_transition_type_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY(etype == 0)) {
    static const GEnumValue values[] = {
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE, "EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE", "none" },
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE, "EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE", "crossfade" },
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_RIGHT, "EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_RIGHT", "slide_right" },
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_LEFT, "EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_LEFT", "slide_left" },
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_UP, "EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_UP", "slide_up" },
      { EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_DOWN, "EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_DOWN", "slide_down" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static (g_intern_static_string ("EosPageManagerTransitionType"), values);
  }
  return etype;
}

enum
{
  PROP_0,
  PROP_VISIBLE_PAGE,
  PROP_VISIBLE_PAGE_NAME,
  PROP_TRANSITION_DURATION,
  PROP_TRANSITION_TYPE,
  NPROPS
};

enum
{
  CHILD_PROP_0,
  CHILD_PROP_NAME,
  CHILD_PROP_PAGE_ACTIONS,
  CHILD_PROP_CUSTOM_TOOLBOX_WIDGET,
  CHILD_PROP_LEFT_TOPBAR_WIDGET,
  CHILD_PROP_CENTER_TOPBAR_WIDGET,
  CHILD_PROP_BACKGROUND_URI,
  CHILD_PROP_BACKGROUND_SIZE,
  CHILD_PROP_BACKGROUND_POSITION,
  CHILD_PROP_BACKGROUND_REPEATS,
  NCHILDPROPS
};

static GParamSpec *eos_page_manager_props[NPROPS] = { NULL, };
static GParamSpec *eos_page_manager_child_props[NCHILDPROPS] = { NULL, };

static void
page_info_free (EosPageManagerPageInfo *info)
{
  g_free (info->name);
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
  return g_hash_table_lookup (self->priv->pages_by_widget, page);
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
  return g_hash_table_lookup (self->priv->pages_by_name, name);
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

/* Invariants: number of pages in list and number of pages in pages_by_widget
hash table must be equal; and number of pages in pages_by_name hash table must
be equal or less. This check is expensive, should only be enabled for debugging.
*/
static void
assert_internal_state (EosPageManager *self)
{
#ifdef DEBUG
  guint list_length = g_list_length (self->priv->page_info);
  g_assert_cmpuint (list_length,
                    ==,
                    g_hash_table_size (self->priv->pages_by_widget));
  g_assert_cmpuint (list_length,
                    >=,
                    g_hash_table_size (self->priv->pages_by_name));
#endif
}

static void
set_visible_page_from_info (EosPageManager         *self,
                            EosPageManagerPageInfo *info)
{
  GtkStack *stack = GTK_STACK (self->priv->stack);
  gtk_stack_set_visible_child (stack, info->page);

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

    case PROP_TRANSITION_DURATION:
      g_value_set_uint (value, eos_page_manager_get_transition_duration (self));
      break;

    case PROP_TRANSITION_TYPE:
      g_value_set_enum (value, eos_page_manager_get_transition_type (self));
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

    case PROP_TRANSITION_DURATION:
      eos_page_manager_set_transition_duration (self, g_value_get_uint (value));
      break;

    case PROP_TRANSITION_TYPE:
      eos_page_manager_set_transition_type (self, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_page_manager_dispose (GObject *object)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);

  g_list_foreach (self->priv->page_info, (GFunc)top_bars_unref, NULL);

  G_OBJECT_CLASS (eos_page_manager_parent_class)->dispose (object);
}

static void
eos_page_manager_finalize (GObject *object)
{
  EosPageManager *self = EOS_PAGE_MANAGER (object);

  g_list_foreach (self->priv->page_info, (GFunc)page_info_free, NULL);
  g_hash_table_destroy(self->priv->pages_by_widget);
  g_hash_table_destroy(self->priv->pages_by_name);

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
  info->background_size = g_strdup (DEFAULT_BACKGROUND_SIZE);
  info->background_position = g_strdup (DEFAULT_BACKGROUND_POSITION);
  info->background_repeats = TRUE;
  info->page = new_page;
  self->priv->page_info = g_list_prepend (self->priv->page_info, info);
  g_hash_table_insert (self->priv->pages_by_widget, new_page, info);

  /* If there were no pages yet, then this one must become the visible one */
  if (self->priv->visible_page_info == NULL)
    eos_page_manager_set_visible_page (self, new_page);

  assert_internal_state (self);
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
  g_hash_table_remove (self->priv->pages_by_widget, page);
  if (info->name != NULL)
    g_hash_table_remove (self->priv->pages_by_name, info->name);

  if (self->priv->visible_page_info == info)
    {
      /* If this was the only page */
      if (self->priv->page_info == NULL)
        {
          self->priv->visible_page_info = NULL;
        }
      /* Otherwise set visible page as the first in our list. */
      else
        {
          g_critical ("Removing the currently visible page %p from the page manager.",
                      page);
          EosPageManagerPageInfo *visible_info = g_list_first (self->priv->page_info)->data;
          set_visible_page_from_info (self, visible_info);
        }

    }

  page_info_free (info);

  assert_internal_state (self);
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

    case CHILD_PROP_PAGE_ACTIONS:
      g_value_set_boolean (value,
                           eos_page_manager_get_page_actions (self, child));
      break;

    case CHILD_PROP_CUSTOM_TOOLBOX_WIDGET:
      g_value_set_object (value,
                          eos_page_manager_get_page_custom_toolbox_widget (self,
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
    case CHILD_PROP_NAME:
      eos_page_manager_set_page_name (self, child, g_value_get_string (value));
      break;

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

    case CHILD_PROP_PAGE_ACTIONS:
      eos_page_manager_set_page_actions (self, child,
                                         g_value_get_boolean (value));
      break;

    case CHILD_PROP_CUSTOM_TOOLBOX_WIDGET:
      eos_page_manager_set_page_custom_toolbox_widget (self, child,
                                                       g_value_get_object (value));
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
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosPageManagerPrivate));

  object_class->get_property = eos_page_manager_get_property;
  object_class->set_property = eos_page_manager_set_property;
  object_class->dispose = eos_page_manager_dispose;
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
   * If the page manager has no pages, or if there is a page currently being
   * displayed but it has no name, then this is %NULL.
   */
  eos_page_manager_props[PROP_VISIBLE_PAGE_NAME] =
    g_param_spec_string ("visible-page-name", "Visible page name",
                         "Name of page currently displaying in the page "
                         "manager",
                         "",
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:transition-duration:
   *
   * The time it will take to animate between pages in the page managers, in
   * milliseconds.
   */
  eos_page_manager_props[PROP_TRANSITION_DURATION] =
    g_param_spec_uint ("transition-duration", "Transition duration",
                       "The animation duration, in milliseconds",
                       0, G_MAXUINT,
                       200,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT);


  /**
   * EosPageManager:transition-type:
   *
   * The type of animation to use when switching between pages in the page
   * manager. The pages can crossfade from one to the next, or slide in from
   * any direction.
   */
  eos_page_manager_props[PROP_TRANSITION_TYPE] =
    g_param_spec_enum ("transition-type", "Transition type",
                       "The type of animation used to transition",
                       EOS_TYPE_PAGE_MANAGER_TRANSITION_TYPE,
                       EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE,
                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

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

  /**
   * EosPageManager:page-actions:
   *
   * The actions exported by this page, to be displayed in the action area on
   * the right of the window.
   *
   * <warning><para>Currently, this property is a boolean value. %TRUE means
   *   to display a fake action area, and %FALSE means don't display.
   * </para></warning>
   */
  eos_page_manager_child_props[CHILD_PROP_PAGE_ACTIONS] =
    g_param_spec_boolean ("page-actions", "Page Actions",
                          "Actions the page exports into the action area",
                          FALSE,
                          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosPageManager:custom-toolbox-widget:
   *
   * The custom toolbox widget belonging to this page, to be displayed on the
   * left of the window when the page is displaying. Setting this to %NULL
   * indicates that there should be no toolbox widget.
   *
   * <warning><para>Currently, there is no such thing as a
   *   <emphasis>non-</emphasis>custom toolbox widget.
   * </para></warning>
   */
  eos_page_manager_child_props[CHILD_PROP_CUSTOM_TOOLBOX_WIDGET] =
    g_param_spec_object ("custom-toolbox-widget", "Custom toolbox widget",
                         "Custom toolbox widget displayed left of the page",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

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
  for (count = PROP_0 + 1; count < NCHILDPROPS; count++)
    gtk_container_class_install_child_property (container_class, count,
                                                eos_page_manager_child_props[count]);
}

static void
eos_page_manager_init (EosPageManager *self)
{
  GtkWidget *self_widget = GTK_WIDGET (self);
  self->priv = PAGE_MANAGER_PRIVATE (self);
  self->priv->pages_by_widget = g_hash_table_new (g_direct_hash,
                                                  g_direct_equal);
  self->priv->pages_by_name = g_hash_table_new_full (g_str_hash,
                                                     g_str_equal,
                                                     g_free,
                                                     NULL);

  gtk_widget_set_has_window (self_widget, FALSE);

  /* TODO replace with GtkStack */
  self->priv->stack = gtk_stack_new ();
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

  if (self->priv->visible_page_info == NULL)
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
 * Returns: (allow-none): the name of the page, or %NULL if @self does not have
 * any pages or if the visible page does not have a name.
 */
const gchar *
eos_page_manager_get_visible_page_name (EosPageManager *self)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), NULL);

  if (self->priv->visible_page_info == NULL)
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
  g_return_if_fail (page_name != NULL);

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
 * Returns: (allow-none): the name of @page, or %NULL if @page does not have a
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

  return info->name;
}

/**
 * eos_page_manager_set_page_name:
 * @self: the page manager
 * @page: the page to be renamed
 * @name: (allow-none): the new name for @page
 *
 * Changes the name of @page, which must previously have been added to the
 * page manager.
 * To remove @page's name, pass %NULL for @name.
 * See #EosPageManager:name for more information.
 */
void
eos_page_manager_set_page_name (EosPageManager *self,
                                GtkWidget      *page,
                                const gchar    *name)
{
  EosPageManagerPageInfo *info;

  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (GTK_IS_WIDGET (page));

  /* Two pages with the same name are not allowed */
  if (name != NULL)
    {
      info = find_page_info_by_name (self, name);
      if (info != NULL && info->page != page)
        {
          g_critical ("Not setting page name to \"%s\", because page manager "
                      "already contains a page by that name",
                      name);
          return;
        }
    }

  info = find_page_info_by_widget (self, page);
  if (info == NULL)
    {
      warn_page_widget_not_found (self, page);
      return;
    }

  if (g_strcmp0(info->name, name) == 0)
    return;

  if (info->name != NULL)
    g_hash_table_remove (self->priv->pages_by_name, info->name);
  g_free (info->name);
  info->name = g_strdup (name);
  if (name != NULL)
      g_hash_table_insert (self->priv->pages_by_name, g_strdup (name), info);

  gtk_container_child_notify (GTK_CONTAINER (self), page, "name");

  assert_internal_state (self);
}

/**
 * eos_page_manager_get_page_actions:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Gets whether to display a fake actions area when displaying @page.
 * See #EosPageManager:page-actions for more information.
 *
 * <warning><para>This function is a temporary implementation, do not expect
 *   this API to remain stable.
 * </para></warning>
 *
 * Returns: %TRUE if the fake actions area should be visible when displaying
 * @page, or %FALSE if it should not.
 */
gboolean
eos_page_manager_get_page_actions (EosPageManager *self,
                                   GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), FALSE);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), FALSE);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, FALSE);

  return info->fake_page_actions_visible;
}

/**
 * eos_page_manager_set_page_actions:
 * @self: the page manager
 * @page: the page
 * @actions_visible: whether to display an action area beside @page
 *
 * Sets whether to display a fake actions area when displaying @page.
 * See #EosPageManager:page-actions for more information.
 *
 * <warning><para>This function is a temporary implementation, do not expect
 *   this API to remain stable.
 * </para></warning>
 */
void
eos_page_manager_set_page_actions (EosPageManager *self,
                                   GtkWidget      *page,
                                   gboolean        actions_visible)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (info->fake_page_actions_visible == actions_visible)
    return;

  info->fake_page_actions_visible = actions_visible;

  gtk_container_child_notify (GTK_CONTAINER (self), page, "page-actions");
}

/**
 * eos_page_manager_get_page_custom_toolbox_widget:
 * @self: the page manager
 * @page: the page to be queried
 *
 * Retrieves @page's custom toolbox widget, if it has one.
 * See #EosPageManager:custom-toolbox-widget for more information.
 *
 * <note><para>
 *   Currently, there is no possible way to have a non-custom toolbox widget.
 * </para></note>
 *
 * Returns: (transfer none): the custom toolbox #GtkWidget of @page, or %NULL if
 * there is none.
 */
GtkWidget *
eos_page_manager_get_page_custom_toolbox_widget (EosPageManager *self,
                                                 GtkWidget      *page)
{
  g_return_val_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self), NULL);
  g_return_val_if_fail (page != NULL && GTK_IS_WIDGET (page), NULL);

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_val_if_fail (info != NULL, NULL);

  return info->custom_toolbox_widget;
}

/**
 * eos_page_manager_set_page_custom_toolbox_widget:
 * @self: the page manager
 * @page: the page
 * @custom_toolbox_widget: (allow-none): custom toolbox widget for @page
 *
 * Sets the custom toolbox widget to display to the left of @page.
 * See #EosPageManager:custom-toolbox-widget for more information.
 *
 * <note><para>
 *   Currently, there is no possible way to have a non-custom toolbox widget.
 * </para></note>
 */
void
eos_page_manager_set_page_custom_toolbox_widget (EosPageManager *self,
                                                 GtkWidget      *page,
                                                 GtkWidget      *custom_toolbox_widget)
{
  g_return_if_fail (self != NULL && EOS_IS_PAGE_MANAGER (self));
  g_return_if_fail (page != NULL && GTK_IS_WIDGET (page));
  g_return_if_fail (custom_toolbox_widget == NULL ||
                    GTK_IS_WIDGET (custom_toolbox_widget));

  EosPageManagerPageInfo *info = find_page_info_by_widget (self, page);
  g_return_if_fail (info != NULL);

  if (info->custom_toolbox_widget == custom_toolbox_widget)
    return;

  if (info->custom_toolbox_widget)
    g_object_unref (info->custom_toolbox_widget);

  g_object_ref (custom_toolbox_widget);
  info->custom_toolbox_widget = custom_toolbox_widget;

  gtk_container_child_notify (GTK_CONTAINER (self), page,
                              "custom-toolbox-widget");
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

  assert_internal_state (self);
}

/**
 * eos_page_manager_get_transition_duration:
 * @self: the page manager
 *
 * Gets the animation duration of page transitions, in milliseconds. See
 * #EosPageManager:transition-duration for more information.
 *
 * Returns: the current transition time of the page manager.
 */
guint
eos_page_manager_get_transition_duration (EosPageManager *self)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), 0);

  return gtk_stack_get_transition_duration (GTK_STACK (self->priv->stack));
}

/**
 * eos_page_manager_set_transition_duration:
 * @self: the page manager
 * @duration: the duration of page transitions, in milliseconds
 *
 * Sets the animation duration of page transitions, in milliseconds. See
 * #EosPageManager:transition-duration for more information.
 */
void
eos_page_manager_set_transition_duration (EosPageManager *self,
                                          guint           duration)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));

  gtk_stack_set_transition_duration (GTK_STACK (self->priv->stack), duration);
  g_object_notify (G_OBJECT (self), "transition-duration");
}

/**
 * eos_page_manager_get_transition_type:
 * @self: the page manager
 *
 * Gets the animation type of page transitions. See
 * #EosPageManager:transition-type for more information.
 *
 * Returns: the current transition type of the page manager.
 */
EosPageManagerTransitionType
eos_page_manager_get_transition_type (EosPageManager *self)
{
  g_return_val_if_fail (EOS_IS_PAGE_MANAGER (self), EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE);

  return self->priv->transition_type;
}


/**
 * eos_page_manager_set_transition_type:
 * @self: the page manager
 * @transition_type: the type of page transitions
 *
 * Sets the animation type of page transitions. See
 * #EosPageManager:transition-type for more information.
 */
void
eos_page_manager_set_transition_type (EosPageManager                *self,
                                      EosPageManagerTransitionType   transition_type)
{
  g_return_if_fail (EOS_IS_PAGE_MANAGER (self));

  self->priv->transition_type = transition_type;
  GtkStackTransitionType gtk_stack_transition;
  switch (transition_type)
    {
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE:
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE:
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_RIGHT:
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_LEFT:
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_UP:
    case EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_DOWN:
      gtk_stack_transition = (GtkStackTransitionType)self->priv->transition_type;
      break;
    default:
      gtk_stack_transition = GTK_STACK_TRANSITION_TYPE_NONE;
      break;
    }
  gtk_stack_set_transition_type (GTK_STACK (self->priv->stack),
                                 gtk_stack_transition);
  g_object_notify (G_OBJECT (self), "transition-type");
}

/*
 * eos_page_manager_get_gtk_stack_transition_type:
 * @self: the page manager
 *
 * Gets the internal gtk_stack transition type used to animate the page manager.
 */
GtkStackTransitionType
eos_page_manager_get_gtk_stack_transition_type (EosPageManager *self)
{
  return gtk_stack_get_transition_type (GTK_STACK (self->priv->stack));
}
