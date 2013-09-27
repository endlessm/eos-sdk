/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "string.h"
#include "eoswindow.h"

#include "eosapplication.h"
#include "eospagemanager.h"
#include "eospagemanager-private.h"
#include "eostopbar-private.h"
#include "eosmainarea-private.h"

#include <gtk/gtk.h>
#include <pstack.h>

/**
 * SECTION:window
 * @short_description: A window for your application
 * @title: Window
 *
 * The #EosWindow class is where you put your application's user interface.
 * You should create a class that extends #EosWindow.
 *
 * Create the interface in your window class's _init() function, like this:
 * |[
 * const SmokeGrinderWindow = new Lang.Class({
 *     Name: 'SmokeGrinderWindow',
 *     Extends: Endless.Window,
 *
 *     _init(): function (props) {
 *         this.parent(props);
 *         this._button = Gtk.Button({label: 'Push me'});
 *         this.add(this._button);
 *     },
 * });
 * ]|
 */

#define BACKGROUND_FRAME_NAME_TEMPLATE "_eos-window-background-%d"

#define TRANSPARENT_FRAME_CSS_PROPERTIES "{ background-image: none;\n" \
                                          " background-color: transparent\n;" \
                                          " border-width: 0px; }\n"

#define BACKGROUND_FRAME_CSS_PROPERTIES_TEMPLATE "{ background-image: url(\"%s\");\n" \
                                                  " background-size: %s;\n" \
                                                  " background-position: %s;\n" \
                                                  " background-repeat: %s;\n" \
                                                  " border-width: 0px; }\n"

#define CSS_TEMPLATE "#%s %s #%s %s"

#define _EOS_TOP_BAR_EDGE_FINISHING_HEIGHT_PX 2

G_DEFINE_TYPE (EosWindow, eos_window, GTK_TYPE_APPLICATION_WINDOW)

#define WINDOW_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_WINDOW, EosWindowPrivate))

struct _EosWindowPrivate
{
  EosApplication *application;

  GtkWidget *top_bar;
  GtkWidget *main_area;
  GtkWidget *overlay;
  GtkWidget *edge_finishing;
  GtkWidget *current_background;
  GtkWidget *next_background;
  GtkWidget *background_stack;

  EosPageManager *page_manager;

  /* For keeping track of what to display alongside the current page */
  GtkWidget *current_page;
  gulong visible_page_property_handler;
  GtkCssProvider *background_provider;
  gchar *current_background_css_props;
};

enum
{
  PROP_0,
  PROP_APPLICATION,
  PROP_PAGE_MANAGER,
  NPROPS
};

static GParamSpec *eos_window_props[NPROPS] = { NULL, };

static void
override_background_css(EosWindow *self, gchar *background_css)
{
  // Override the css
  GtkStyleProvider *provider =
    GTK_STYLE_PROVIDER (self->priv->background_provider);
  GdkScreen *screen = gdk_screen_get_default ();
  GError *error = NULL;
  gtk_style_context_remove_provider_for_screen (screen, provider);
  gtk_css_provider_load_from_data (self->priv->background_provider,
                                   background_css, -1, &error);
  gtk_style_context_add_provider_for_screen (screen, provider,
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

/*
 * update_page_actions:
 * @self: the window
 *
 * Ensures that the currently shown state of the action area is in line with
 * the child properties of the currently showing page.
 */
static void
update_page_actions (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  EosMainArea *ma = EOS_MAIN_AREA (self->priv->main_area);
  GtkWidget *page = self->priv->current_page;

  if (page != NULL)
    {
      gboolean fake_action_area = eos_page_manager_get_page_actions (pm, page);
      eos_main_area_set_actions (ma, fake_action_area);
    }
  else
    {
      eos_main_area_set_actions (ma, FALSE);
    }
}

/*
 * update_page_toolbox:
 * @self: the window
 *
 * Ensures that the currently shown state of the toolbox is in line with
 * the child properties of the currently showing page.
 */
static void
update_page_toolbox (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  EosMainArea *ma = EOS_MAIN_AREA (self->priv->main_area);
  GtkWidget *page = self->priv->current_page;

  if (page != NULL)
    {
      GtkWidget *custom_toolbox_widget =
        eos_page_manager_get_page_custom_toolbox_widget (pm, page);
      eos_main_area_set_toolbox (ma, custom_toolbox_widget);
    }
  else
    {
      eos_main_area_set_toolbox (ma, NULL);
    }
}

/**
 * update_page_left_topbar:
 * @self: the window
 * 
 * Ensures that the currently shown state of the left topbar is in line with
 * the child properties of the currently showing page.
 */
static void
update_page_left_topbar (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  EosTopBar *tb = EOS_TOP_BAR (self->priv->top_bar);
  GtkWidget *page = self->priv->current_page;

  if (page != NULL)
    {
      GtkWidget *left_topbar_widget = 
        GTK_WIDGET (eos_page_manager_get_page_left_topbar_widget (pm, page));
      eos_top_bar_set_left_widget (tb, left_topbar_widget);
    }
  else
    {
      eos_top_bar_set_left_widget (tb, NULL);
    }
}

/**
 * update_page_center_topbar:
 * @self: the window
 * 
 * Ensures that the currently-shown state of the center topbar is in line with
 * the child properties of the currently-showing page.
 */
static void
update_page_center_topbar (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  EosTopBar *tb = EOS_TOP_BAR (self->priv->top_bar);
  GtkWidget *page = self->priv->current_page;

  if (page != NULL)
    {
      GtkWidget *center_topbar_widget =
        GTK_WIDGET (eos_page_manager_get_page_center_topbar_widget (pm, page));
      eos_top_bar_set_center_widget (tb, center_topbar_widget);
    }
  else
    {
      eos_top_bar_set_center_widget (tb, NULL);
    }
}

static void
sync_stack_animation (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  p_stack_set_transition_type (P_STACK (self->priv->background_stack),
                               eos_page_manager_get_pstack_transition_type (pm));
  p_stack_set_transition_duration (P_STACK (self->priv->background_stack),
                                   eos_page_manager_get_transition_duration (pm));
}

// Helper to generate frame css override
static gchar *
format_background_css (EosPageManager *pm,
                       GtkWidget *page)
{
  const gchar *background_uri = eos_page_manager_get_page_background_uri (pm, page);
  const gchar *background_size = eos_page_manager_get_page_background_size (pm, page);
  const gchar *background_position = eos_page_manager_get_page_background_position (pm, page);
  gboolean background_repeats = eos_page_manager_get_page_background_repeats (pm, page);
  const gchar *background_repeats_string = background_repeats ? "repeat" : "no-repeat";

  // If background uri is NULL (unset) our css override forces the frame to be
  // transparent. So any css styling of EosWindow will "show through" the
  // pages.
  if (background_uri == NULL)
    return TRANSPARENT_FRAME_CSS_PROPERTIES;
  return g_strdup_printf (BACKGROUND_FRAME_CSS_PROPERTIES_TEMPLATE,
                          background_uri,
                          background_size,
                          background_position,
                          background_repeats_string);
}

/*
 * update_page_background:
 * @self: the window
 *
 * Ensures that the window's background image is in line with the currently
 * showing page and its child properties.
 */
static void
update_page_background (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);
  GtkWidget *page = self->priv->current_page;
  // If no page set, do not transition
  if (page == NULL)
    return;
  // Set up css override for transition background...
  gchar *next_background_css_props = format_background_css (pm,
                                                            page);
  // If page background are exactly the same, do not transition
  if (g_strcmp0 (self->priv->current_background_css_props, next_background_css_props) == 0)
    return;
  gchar *background_css = g_strdup_printf(CSS_TEMPLATE,
                                          gtk_widget_get_name (self->priv->current_background),
                                          self->priv->current_background_css_props,
                                          gtk_widget_get_name (self->priv->next_background),
                                          next_background_css_props);
  override_background_css (self, background_css);
  p_stack_set_visible_child (P_STACK (self->priv->background_stack),
                                      self->priv->next_background);
  // Swap our background frames for next animation
  GtkWidget *temp = self->priv->next_background;
  self->priv->next_background = self->priv->current_background;
  self->priv->current_background = temp;
  self->priv->current_background_css_props = next_background_css_props;
}

/*
 * update_visible_page_properties:
 * @widget: the page
 * @child_property: the property that changed
 * @user_data: pointer to the window
 *
 * Updates the currently displaying page when one of its child properties
 * changes.
 */
static void
update_visible_page_properties (GtkWidget  *widget,
                                GParamSpec *child_property,
                                gpointer    data)
{
  EosWindow *self = (EosWindow *)data;
  const gchar *property_name = child_property->name;
  if (g_strcmp0 (property_name, "page-actions") == 0)
    update_page_actions (self);
  else if (g_strcmp0 (property_name, "custom-toolbox-widget") == 0)
    update_page_toolbox (self);
  else if (g_strcmp0 (property_name, "left-topbar-widget") == 0)
    update_page_left_topbar (self);
  else if (g_strcmp0 (property_name, "center-topbar-widget") == 0)
    update_page_center_topbar (self);
  else if (g_strcmp0 (property_name, "background-uri") == 0
           || g_strcmp0 (property_name, "background-size") == 0
           || g_strcmp0 (property_name, "background-position") == 0
           || g_strcmp0 (property_name, "background-repeats") == 0)
    update_page_background (self);
}

/*
 * update_page:
 * @self: the window
 *
 * Ensures that the state of the window, the window's main area and top bar are 
 * in line with the currently showing page and its child properties.
 */
static void
update_page (EosWindow *self)
{
  EosPageManager *pm = EOS_PAGE_MANAGER (self->priv->page_manager);

  if (self->priv->current_page)
    {
      g_signal_handler_disconnect (self->priv->current_page,
                                   self->priv->visible_page_property_handler);
    }
  self->priv->current_page = eos_page_manager_get_visible_page (pm);

  update_page_actions (self);
  update_page_toolbox (self);
  sync_stack_animation (self);
  update_page_left_topbar (self);
  update_page_center_topbar (self);
  update_page_background (self);
  p_stack_set_transition_type (P_STACK (self->priv->background_stack),
                               P_STACK_TRANSITION_TYPE_NONE);

  if (self->priv->current_page)
    {
      self->priv->visible_page_property_handler =
        g_signal_connect (self->priv->current_page,
                          "child-notify",
                          G_CALLBACK (update_visible_page_properties),
                          self);
    }
}

static void
eos_window_get_property (GObject    *object,
                         guint       property_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  EosWindow *self = EOS_WINDOW (object);

  switch (property_id)
    {
    case PROP_APPLICATION:
      g_value_set_object (value, self->priv->application);
      break;

    case PROP_PAGE_MANAGER:
      g_value_set_object (value, eos_window_get_page_manager (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
set_application (EosWindow *self,
                 EosApplication *application)
{
  self->priv->application = application;
  gtk_window_set_application (GTK_WINDOW (self),
                              GTK_APPLICATION (self->priv->application));
  if (self->priv->application == NULL)
    {
      g_error ("In order to create a window, you must have an application "
               "for it to connect to.");
      return;
    }
}

static void
eos_window_set_property (GObject      *object,
                         guint         property_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  EosWindow *self = EOS_WINDOW (object);

  switch (property_id)
    {
    case PROP_APPLICATION:
      set_application (self, g_value_get_object (value));
      break;

    case PROP_PAGE_MANAGER:
      eos_window_set_page_manager (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

/* Piggy-back on the parent class's get_preferred_height(), but add the
height of our top bar. Do not assume any borders on the top bar. */
static void
eos_window_get_preferred_height (GtkWidget *widget,
                                 int *minimum_height,
                                 int *natural_height)
{
  EosWindow *self = EOS_WINDOW (widget);
  int top_bar_minimum, top_bar_natural;

  GTK_WIDGET_CLASS (eos_window_parent_class)->get_preferred_height (widget,
    minimum_height, natural_height);
  gtk_widget_get_preferred_height (self->priv->top_bar,
                                   &top_bar_minimum, &top_bar_natural);
  if (minimum_height != NULL)
    *minimum_height += top_bar_minimum;
  if (natural_height != NULL)
    *natural_height += top_bar_natural;
}

/* Remove space for our top bar from the allocation before doing a normal
size_allocate(). Do not assume any borders on the top bar. */
static void
eos_window_size_allocate (GtkWidget *widget,
                          GtkAllocation *allocation)
{
  EosWindow *self = EOS_WINDOW (widget);
  GtkWidget *child;
  GtkAllocation child_allocation = *allocation;

  gtk_widget_set_allocation (widget, allocation);

  if (self->priv->top_bar != NULL)
    {
      int top_bar_natural;
      GtkAllocation top_bar_allocation = *allocation;

      gtk_widget_get_preferred_height (self->priv->top_bar,
                                       NULL, &top_bar_natural);
      top_bar_allocation.height = MIN(top_bar_natural, allocation->height);
      child_allocation.y += top_bar_allocation.height;
      child_allocation.height -= top_bar_allocation.height;

      gtk_widget_size_allocate (self->priv->top_bar, &top_bar_allocation);
    }

  /* We can't chain up to GtkWindow's implementation of size_allocate() here,
  because it always assumes that its child begins at (0, 0). */
  child = gtk_bin_get_child (GTK_BIN (self));
  if (child != NULL)
      gtk_widget_size_allocate (child, &child_allocation);
}

static void
eos_window_map (GtkWidget *widget)
{
  EosWindow *self = EOS_WINDOW (widget);

  GTK_WIDGET_CLASS (eos_window_parent_class)->map (widget);
  if (self->priv->top_bar != NULL
      && gtk_widget_get_visible (self->priv->top_bar))
    {
      gtk_widget_map (self->priv->top_bar);
    }
}

static void
eos_window_unmap (GtkWidget *widget)
{
  EosWindow *self = EOS_WINDOW (widget);

  GTK_WIDGET_CLASS (eos_window_parent_class)->unmap (widget);
  if (self->priv->top_bar != NULL)
    gtk_widget_unmap (self->priv->top_bar);
}

static void
eos_window_show (GtkWidget *widget)
{
  EosWindow *self = EOS_WINDOW (widget);

  GTK_WIDGET_CLASS (eos_window_parent_class)->show (widget);
  if (self->priv->top_bar != NULL)
    gtk_widget_show_all (self->priv->top_bar);
    
}

/* The top bar is an internal child, so include it in our list of internal
children. */
static void
eos_window_forall (GtkContainer *container,
                   gboolean include_internals,
                   GtkCallback callback,
                   gpointer callback_data)
{
  EosWindow *self = EOS_WINDOW (container);

  if (include_internals && self->priv->top_bar != NULL)
    (*callback) (self->priv->top_bar, callback_data);
  GTK_CONTAINER_CLASS (eos_window_parent_class)->forall (container,
                                                         include_internals,
                                                         callback,
                                                         callback_data);
}

/* Our default delete event handler destroys the window. */
static gboolean
eos_window_default_delete (GtkWidget* window,
                     gpointer user_data)
{
  gtk_widget_destroy (GTK_WIDGET (window));
  return FALSE;
}

static void
eos_window_class_init (EosWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosWindowPrivate));

  object_class->get_property = eos_window_get_property;
  object_class->set_property = eos_window_set_property;
  /* Overriding the following six functions is because we treat the top bar as
  an "internal" child. This will not be necessary any more if we use
  gtk_window_set_titlebar(), available from GTK >= 3.10. But for now we are
  targeting GTK 3.8. Issue: [endlessm/eos-sdk#28] */
  widget_class->get_preferred_height = eos_window_get_preferred_height;
  widget_class->size_allocate = eos_window_size_allocate;
  widget_class->map = eos_window_map;
  widget_class->unmap = eos_window_unmap;
  widget_class->show = eos_window_show;
  container_class->forall = eos_window_forall;

  /**
   * EosWindow:application:
   *
   * The #EosApplication that this window is associated with. See also
   * #GtkWindow:application; the difference is that #EosWindow:application
   * cannot be %NULL and must be an #EosApplication.
   */
  eos_window_props[PROP_APPLICATION] =
    g_param_spec_object ("application", "Application",
                         "Application associated with this window",
                         EOS_TYPE_APPLICATION,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  /**
   * EosWindow:page-manager:
   *
   * The #EosPageManager that controls the flow of this window's application.
   */
  eos_window_props[PROP_PAGE_MANAGER] =
    g_param_spec_object ("page-manager", "Page manager",
                         "Page manager associated with this window",
                         EOS_TYPE_PAGE_MANAGER,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS, eos_window_props);
}

#if GTK_CHECK_VERSION (3, 10, 0)
#define our_window_close(w)     gtk_window_close (GTK_WINDOW (w))
#else
static gboolean
queue_close (gpointer user_data)
{
  GtkWidget *window = user_data;

  GdkEvent *event = gdk_event_new (GDK_DELETE);

  event->any.window = gtk_widget_get_window (window);
  event->any.send_event = TRUE;

  gtk_main_do_event (event);

  gdk_event_free (event);

  return G_SOURCE_REMOVE;
}

static void
our_window_close (GtkWidget *window)
{
  if (!gtk_widget_get_realized (window))
    return;

  gdk_threads_add_idle (queue_close, window);
}
#endif /* GTK_CHECK_VERSION (3, 10, 0) */

static void
on_minimize_clicked_cb (GtkWidget* top_bar,
                        gpointer user_data)
{
  if (user_data != NULL)
    {
      gtk_window_iconify (GTK_WINDOW (user_data));
    }
}

static void
on_close_clicked_cb (GtkWidget* top_bar,
                     gpointer user_data)
{
  GtkWidget *window = gtk_widget_get_toplevel (top_bar);

  our_window_close (window);
}

/* Make sure that the edge finishing does not catch input events */
static void
after_edge_finishing_realize_cb (GtkWidget *edge_finishing)
{
  cairo_rectangle_int_t empty = { 0, 0, 0, 0 };
  cairo_region_t *empty_region = cairo_region_create_rectangle (&empty);
  gdk_window_input_shape_combine_region (gtk_widget_get_window (edge_finishing),
                                         empty_region, 0, 0);
  cairo_region_destroy (empty_region);
}

/* Draw the edge finishing on the two lines on top of the window's content;
see eos_top_bar_draw() for the two lines inside the top bar */
static gboolean
on_edge_finishing_draw_cb (GtkWidget *edge_finishing,
                           cairo_t   *cr)
{
  gint width = gtk_widget_get_allocated_width (edge_finishing);
  cairo_set_line_width (cr, 1.0);
  /* Shadow 1: #000000, opacity 15% */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.15);
  cairo_move_to (cr, 0, 0.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);
  /* Shadow 2: #000000, opacity 5% */
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.05);
  cairo_move_to (cr, 0, 1.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);

  return GDK_EVENT_PROPAGATE;
}

static void
eos_window_init (EosWindow *self)
{
  self->priv = WINDOW_PRIVATE (self);

  self->priv->top_bar = eos_top_bar_new ();
  gtk_widget_set_parent (self->priv->top_bar, GTK_WIDGET (self));

  self->priv->overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (self), self->priv->overlay);

  self->priv->background_stack = p_stack_new ();
  gtk_container_add (GTK_CONTAINER (self->priv->overlay), self->priv->background_stack);

  gchar *background_name1 = g_strdup_printf (BACKGROUND_FRAME_NAME_TEMPLATE, 1);
  self->priv->next_background = g_object_new (GTK_TYPE_FRAME, "name", background_name1, NULL);
  gtk_container_add (GTK_CONTAINER (self->priv->background_stack), self->priv->next_background);

  // Add the current background to the stack second. I think the latest added
  // will be the first visible page in the stack
  gchar *background_name0 = g_strdup_printf (BACKGROUND_FRAME_NAME_TEMPLATE, 0);
  self->priv->current_background = g_object_new (GTK_TYPE_FRAME, "name", background_name0, NULL);
  gtk_container_add (GTK_CONTAINER (self->priv->background_stack), self->priv->current_background);

  self->priv->background_provider = gtk_css_provider_new ();
  // We start all the background frames transparent with no styling
  self->priv->current_background_css_props = TRANSPARENT_FRAME_CSS_PROPERTIES;
  gchar *background_css = g_strdup_printf(CSS_TEMPLATE,
                                          gtk_widget_get_name (self->priv->current_background),
                                          TRANSPARENT_FRAME_CSS_PROPERTIES,
                                          gtk_widget_get_name (self->priv->next_background),
                                          TRANSPARENT_FRAME_CSS_PROPERTIES);
  override_background_css (self, background_css);

  self->priv->main_area = eos_main_area_new ();
  gtk_overlay_add_overlay (GTK_OVERLAY (self->priv->overlay), self->priv->main_area);

  self->priv->edge_finishing = gtk_drawing_area_new ();
  gtk_widget_set_vexpand (self->priv->edge_finishing, FALSE);
  gtk_widget_set_valign (self->priv->edge_finishing, GTK_ALIGN_START);
  /* has_window == FALSE is necessary for not catching input events */
  gtk_widget_set_has_window (self->priv->edge_finishing, FALSE);
  gtk_widget_set_size_request (self->priv->edge_finishing,
                               -1, _EOS_TOP_BAR_EDGE_FINISHING_HEIGHT_PX);
  g_signal_connect_after (self->priv->edge_finishing, "realize",
                          G_CALLBACK (after_edge_finishing_realize_cb), NULL);
  g_signal_connect (self->priv->edge_finishing, "draw",
                    G_CALLBACK (on_edge_finishing_draw_cb), NULL);
  gtk_overlay_add_overlay (GTK_OVERLAY (self->priv->overlay),
                           self->priv->edge_finishing);

  gtk_window_set_decorated (GTK_WINDOW (self), FALSE);
  gtk_window_maximize (GTK_WINDOW (self));

  g_signal_connect (self->priv->top_bar, "minimize-clicked",
                    G_CALLBACK (on_minimize_clicked_cb), self);
  g_signal_connect (self->priv->top_bar, "close-clicked",
                    G_CALLBACK (on_close_clicked_cb), self);

  eos_window_set_page_manager (self,
                               EOS_PAGE_MANAGER (eos_page_manager_new ()));
}

/* Public API */

/**
 * eos_window_new:
 * @application: the #EosApplication that the window belongs to.
 *
 * Create a window. It is invisible by default.
 *
 * Returns: a pointer to the window.
 */
GtkWidget *
eos_window_new (EosApplication *application)
{
  return GTK_WIDGET (g_object_new (EOS_TYPE_WINDOW,
                                   "application", application,
                                   NULL));
}

/**
 * eos_window_get_page_manager:
 * @self: the window
 *
 * Stub
 *
 * Returns: (transfer none) (allow-none): a pointer to the current page manager,
 * or %NULL if there is no page manager set.
 */
EosPageManager *
eos_window_get_page_manager (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), NULL);

  return self->priv->page_manager;
}

/**
 * eos_window_set_page_manager:
 * @self: the window
 * @page_manager: the page manager
 *
 * Stub
 */
void
eos_window_set_page_manager (EosWindow *self,
                             EosPageManager *page_manager)
{
  g_return_if_fail (self != NULL && EOS_IS_WINDOW (self));
  g_return_if_fail (page_manager != NULL && EOS_IS_PAGE_MANAGER (page_manager));

  EosMainArea *main_area = EOS_MAIN_AREA (self->priv->main_area);

  self->priv->page_manager = page_manager;

  eos_main_area_set_content (main_area,
                             GTK_WIDGET (self->priv->page_manager));

  update_page (self);

  g_signal_connect_swapped (self->priv->page_manager, "notify::visible-page",
                            G_CALLBACK (update_page), self);
}
