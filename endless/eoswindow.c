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
 *
 * We will use an application-configurable base font size for application-
 * configurable resolution and scale up/down from there for different screen sizes.
 *
 * Font scaling can be enabled by setting #EosWindow:font-scaling-active to
 * true. Font scaling is turned off and the property is false by default.
 *
 * The default font size by which font scaling will occur can be set by
 * #EosWindow:font-scaling-default-size.
 *
 * The default window resolution height by which font scaling will occur can be
 * set by #EosWindow:font-scaling-default-window-size.
 *
 * The default minimum font size under which a font will never scale can be set
 * by #EosWindow:font-scaling-min-font-size.
 *
 * The calculated minimum font size by which children widgets will scale and can
 * be retrieved by #EosWindow:font-scaling-calculated-font-size. This property is
 * only readable and is only set by #EosWindow internally.
 *
 * For instance, supose we have a default font size of 12px, a default window size
 * of 720px, and a window allocation of 360px. The calculated font pixel size
 * will be 12px * (360px / 720px) = 6px. A corresponding CSS font-size of 1em will
 * be equivalent to 6 px. A CSS font-size of 0.5em will be equivalent to 3px. If the
 * window is resized to a height of 720px, then the calculated pixel size will
 * be 12px, and the CSS font-size of 1em will be equivalent to 12px. A CSS
 * font-size of 0.5em will be equivalent to 6px. If the minimum font size is set
 * to 12px, then the font-size will be forced to 12px, ignoring the calculated font
 * size of 6px.
 */

#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

#define BACKGROUND_FRAME_NAME_TEMPLATE "_eos-window-background-%d"

#define FONT_SIZE_TEMPLATE "EosWindow { font-size: %fpx; }"

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

typedef struct {
  EosApplication *application;

  GtkWidget *top_bar;
  GtkWidget *main_area;
  GtkWidget *overlay;
  GtkSizeGroup *overlay_size_group;
  GtkWidget *edge_finishing;
  GtkWidget *current_background;
  GtkWidget *next_background;
  GtkWidget *background_stack;

  EosPageManager *page_manager;

  gboolean maximized;

  /* For scaling base font-size */
  GtkCssProvider *font_size_provider;
  gboolean font_scaling_active;
  gint font_scaling_default_size;
  gint font_scaling_default_window_size;
  gint font_scaling_min_font_size;
  gdouble font_scaling_calculated_font_size;

  /* For keeping track of what to display alongside the current page */
  GtkWidget *current_page;
  gulong visible_page_property_handler;
  GtkCssProvider *background_provider;
  gchar *current_background_css_props;
} EosWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosWindow, eos_window, GTK_TYPE_APPLICATION_WINDOW)

enum
{
  PROP_0,
  PROP_APPLICATION,
  PROP_PAGE_MANAGER,
  PROP_FONT_SCALING_ACTIVE,
  PROP_FONT_SCALING_DEFAULT_SIZE,
  PROP_FONT_SCALING_DEFAULT_WINDOW_SIZE,
  PROP_FONT_SCALING_MIN_FONT_SIZE,
  PROP_FONT_SCALING_CALCULATED_FONT_SIZE,
  NPROPS
};

static GParamSpec *eos_window_props[NPROPS] = { NULL, };

static void
override_background_css(EosWindow *self, gchar *background_css)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  // Override the css
  GtkStyleProvider *provider =
    GTK_STYLE_PROVIDER (priv->background_provider);
  GdkScreen *screen = gdk_screen_get_default ();
  GError *error = NULL;
  gtk_style_context_remove_provider_for_screen (screen, provider);
  gtk_css_provider_load_from_data (priv->background_provider,
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  EosMainArea *ma = EOS_MAIN_AREA (priv->main_area);
  GtkWidget *page = priv->current_page;

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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  EosMainArea *ma = EOS_MAIN_AREA (priv->main_area);
  GtkWidget *page = priv->current_page;

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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  EosTopBar *tb = EOS_TOP_BAR (priv->top_bar);
  GtkWidget *page = priv->current_page;

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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  EosTopBar *tb = EOS_TOP_BAR (priv->top_bar);
  GtkWidget *page = priv->current_page;

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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  gtk_stack_set_transition_type (GTK_STACK (priv->background_stack),
                                 eos_page_manager_get_gtk_stack_transition_type (pm));
  gtk_stack_set_transition_duration (GTK_STACK (priv->background_stack),
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
    return g_strdup (TRANSPARENT_FRAME_CSS_PROPERTIES);
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);
  GtkWidget *page = priv->current_page;
  // If no page set, do not transition
  if (page == NULL)
    return;
  // Set up css override for transition background...
  gchar *next_background_css_props = format_background_css (pm,
                                                            page);
  // If page background are exactly the same, do not transition
  if (g_strcmp0 (priv->current_background_css_props, next_background_css_props) == 0)
    return;
  gchar *background_css = g_strdup_printf(CSS_TEMPLATE,
                                          gtk_widget_get_name (priv->current_background),
                                          priv->current_background_css_props,
                                          gtk_widget_get_name (priv->next_background),
                                          next_background_css_props);
  override_background_css (self, background_css);
  gtk_stack_set_visible_child (GTK_STACK (priv->background_stack),
                               priv->next_background);
  g_free (background_css);
  // Swap our background frames for next animation
  GtkWidget *temp = priv->next_background;
  priv->next_background = priv->current_background;
  priv->current_background = temp;

  g_free (priv->current_background_css_props);
  priv->current_background_css_props = next_background_css_props;
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);

  if (priv->current_page)
    {
      g_signal_handler_disconnect (priv->current_page,
                                   priv->visible_page_property_handler);
    }
  priv->current_page = eos_page_manager_get_visible_page (pm);

  update_page_actions (self);
  update_page_toolbox (self);
  sync_stack_animation (self);
  update_page_left_topbar (self);
  update_page_center_topbar (self);
  update_page_background (self);
  gtk_stack_set_transition_type (GTK_STACK (priv->background_stack),
                                 GTK_STACK_TRANSITION_TYPE_NONE);

  if (priv->current_page)
    {
      priv->visible_page_property_handler =
        g_signal_connect (priv->current_page,
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  switch (property_id)
    {
    case PROP_APPLICATION:
      g_value_set_object (value, priv->application);
      break;

    case PROP_PAGE_MANAGER:
      g_value_set_object (value, eos_window_get_page_manager (self));
      break;

    case PROP_FONT_SCALING_ACTIVE:
      g_value_set_boolean (value, priv->font_scaling_active);
      break;

    case PROP_FONT_SCALING_DEFAULT_SIZE:
      g_value_set_int (value, priv->font_scaling_default_size);
      break;

    case PROP_FONT_SCALING_DEFAULT_WINDOW_SIZE:
      g_value_set_int (value, priv->font_scaling_default_window_size);
      break;

    case PROP_FONT_SCALING_MIN_FONT_SIZE:
      g_value_set_int (value, priv->font_scaling_min_font_size);
      break;

    case PROP_FONT_SCALING_CALCULATED_FONT_SIZE:
      g_value_set_double (value, priv->font_scaling_calculated_font_size);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
set_application (EosWindow *self,
                 EosApplication *application)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  priv->application = application;
  gtk_window_set_application (GTK_WINDOW (self),
                              GTK_APPLICATION (priv->application));
  if (priv->application == NULL)
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

    case PROP_FONT_SCALING_ACTIVE:
      eos_window_set_font_scaling_active (self, g_value_get_boolean (value));
      break;

    case PROP_FONT_SCALING_DEFAULT_SIZE:
      eos_window_set_font_scaling_default_size (self, g_value_get_int (value));
      break;

    case PROP_FONT_SCALING_DEFAULT_WINDOW_SIZE:
      eos_window_set_font_scaling_default_window_size (self, g_value_get_int (value));
      break;

    case PROP_FONT_SCALING_MIN_FONT_SIZE:
      eos_window_set_font_scaling_min_font_size (self, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_window_finalize (GObject *object)
{
  EosWindow *self = EOS_WINDOW (object);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  g_object_unref (priv->background_provider);
  g_object_unref (priv->font_size_provider);
  g_free (priv->current_background_css_props);

  G_OBJECT_CLASS (eos_window_parent_class)->finalize (object);
}

/* Clamp our size request calls so we never ask for a minimal size greater than
 the available work area. */
static void
clamp_size_request (GtkWidget      *widget,
                    GtkOrientation  orientation,
                    gint            *minimum_size,
                    gint            *natural_size)
{

  if (gtk_widget_get_realized (widget))
    {
      GdkScreen *default_screen = gdk_screen_get_default ();
      GdkWindow *gdkwindow = gtk_widget_get_window (widget);
      int monitor = gdk_screen_get_monitor_at_window (default_screen, gdkwindow);
      GdkRectangle workarea;
      gdk_screen_get_monitor_workarea (default_screen, monitor, &workarea);
      gint available_size = workarea.width;
      gchar *orientation_string = "width";
      if (orientation == GTK_ORIENTATION_VERTICAL)
        {
          available_size = workarea.height;
          orientation_string = "height";
        }

      if (*minimum_size > available_size)
        {
          g_critical ("Requested window %s %d greater than available work area %s %d. " \
                      "Clamping size request to fit. This means there is a bug in your " \
                      "program, and it is not ready for production. Try checking if any " \
                      "of your widgets have minimum size requests that make the page not " \
                      "able to fit on the screen.",
                      orientation_string,
                      *minimum_size,
                      orientation_string,
                      available_size);
          *minimum_size = available_size;
          *natural_size = MAX (*minimum_size, *natural_size);
        }
    }
}

static void
eos_window_get_preferred_width (GtkWidget *widget,
                                gint *minimum_width,
                                gint *natural_width)
{
  GTK_WIDGET_CLASS (eos_window_parent_class)->get_preferred_width (widget,
    minimum_width, natural_width);

  clamp_size_request (widget,
                      GTK_ORIENTATION_HORIZONTAL,
                      minimum_width,
                      natural_width);
}

static void
eos_window_get_preferred_height (GtkWidget *widget,
                                 gint *minimum_height,
                                 gint *natural_height)
{
  GTK_WIDGET_CLASS (eos_window_parent_class)->get_preferred_height (widget,
    minimum_height, natural_height);

  clamp_size_request (widget,
                      GTK_ORIENTATION_VERTICAL,
                      minimum_height,
                      natural_height);
}

/* Updates the base font size depending on the window size. */
static void
eos_window_size_allocate (GtkWidget *window, GtkAllocation *allocation)
{
  EosWindow *self = EOS_WINDOW (window);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  if (priv->font_scaling_active)
    {
      GtkStyleProvider *provider = GTK_STYLE_PROVIDER (priv->font_size_provider);
      priv->font_scaling_calculated_font_size = (gdouble) priv->font_scaling_default_size *
                                ((gdouble) allocation->height / (gdouble) priv->font_scaling_default_window_size);

      if (priv->font_scaling_calculated_font_size < priv->font_scaling_min_font_size)
        priv->font_scaling_calculated_font_size = priv->font_scaling_min_font_size;

      GError *error = NULL;

      gchar *font_size_css = g_strdup_printf (FONT_SIZE_TEMPLATE, priv->font_scaling_calculated_font_size);
      GdkScreen *screen = gdk_screen_get_default ();

      gtk_style_context_remove_provider_for_screen (screen, provider);
      gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider),
                                       font_size_css, -1, &error);
      gtk_style_context_add_provider_for_screen (screen, provider,
                                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

      g_free(font_size_css);
    }

  GTK_WIDGET_CLASS (eos_window_parent_class)->size_allocate (window, allocation);
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

  object_class->get_property = eos_window_get_property;
  object_class->set_property = eos_window_set_property;
  object_class->finalize = eos_window_finalize;
  widget_class->get_preferred_height = eos_window_get_preferred_height;
  widget_class->get_preferred_width = eos_window_get_preferred_width;
  widget_class->size_allocate = eos_window_size_allocate;

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

  /**
   * EosWindow:font-scaling-active:
   *
   * The scaling flag that determines if the windows scale or not.
   */
  eos_window_props[PROP_FONT_SCALING_ACTIVE] =
    g_param_spec_boolean ("font-scaling-active", "Font scaling active",
                         "Whether or not EosWindow objects scale font size",
                         FALSE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  /**
   * EosWindow:font-scaling-default-size:
   *
   * The default font-size by which font scaling will occur. Units are in pixels.
   */
  eos_window_props[PROP_FONT_SCALING_DEFAULT_SIZE] =
    g_param_spec_int ("font-scaling-default-size", "Font scaling default size",
                      "This is the default font-size by which font-size for children widgets will scale",
                      1,
                      G_MAXINT,
                      12,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  /**
   * EosWindow:font-scaling-default-window-size:
   *
   * The base resolution by which font scaling will occur. Units are in pixels.
   */
  eos_window_props[PROP_FONT_SCALING_DEFAULT_WINDOW_SIZE] =
    g_param_spec_int ("font-scaling-default-window-size", "Font scaling default window size",
                      "This is the base resolution by which font-size for children widgets will scale",
                      1,
                      G_MAXINT,
                      1080,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  /**
   * EosWindow:font-scaling-min-font-size:
   *
   * The minimum font-size under which font scaling won't occur. Units are in pixels.
   */
  eos_window_props[PROP_FONT_SCALING_MIN_FONT_SIZE] =
    g_param_spec_int ("font-scaling-min-font-size", "Font scaling default size",
                      "This is the minimum font-size under which font-size for children widgets won't scale",
                      1,
                      G_MAXINT,
                      8,
                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  /**
   * EosWindow:font-scaling-calculated-font-size:
   *
   * The calculated font-size by which children widgets scale. Units are in pixels.
   */
  eos_window_props[PROP_FONT_SCALING_CALCULATED_FONT_SIZE] =
    g_param_spec_double ("font-scaling-calculated-font-size", "Font scaling calculated size",
                      "This is the calculated font-size by which children widgets scale",
                      1,
                      G_MAXDOUBLE,
                      12,
                      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS, eos_window_props);
}

static void
on_minimize_clicked_cb (GtkWidget *top_bar,
                        EosWindow *self)
{
  gtk_window_iconify (GTK_WINDOW (self));
}

static void
on_maximize_clicked_cb (GtkWidget *top_bar,
                        EosWindow *self)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  if (priv->maximized)
    gtk_window_unmaximize (GTK_WINDOW (self));
  else
    gtk_window_maximize (GTK_WINDOW (self));
}

static void
on_close_clicked_cb (GtkWidget *top_bar,
                     EosWindow *self)
{
  gtk_window_close (GTK_WINDOW (self));
}

static void
on_window_state_event_cb (GtkWidget           *widget,
                          GdkEventWindowState *event)
{
  EosWindow *self = EOS_WINDOW (widget);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  GdkWindowState window_state = event->new_window_state;
  priv->maximized = window_state & GDK_WINDOW_STATE_MAXIMIZED;
  eos_top_bar_update_window_maximized (EOS_TOP_BAR (priv->top_bar), priv->maximized);
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  priv->top_bar = eos_top_bar_new ();
  gtk_widget_show_all (priv->top_bar);
  gtk_window_set_titlebar (GTK_WINDOW (self), priv->top_bar);

  priv->overlay = gtk_overlay_new ();
  gtk_container_add (GTK_CONTAINER (self), priv->overlay);

  priv->background_stack = gtk_stack_new ();
  gtk_container_add (GTK_CONTAINER (priv->overlay), priv->background_stack);

  gchar *background_name1 = g_strdup_printf (BACKGROUND_FRAME_NAME_TEMPLATE, 1);
  priv->next_background = g_object_new (GTK_TYPE_FRAME, "name", background_name1, NULL);
  gtk_widget_show (priv->next_background);
  gtk_container_add (GTK_CONTAINER (priv->background_stack), priv->next_background);
  g_free (background_name1);

  // Add the current background to the stack second. I think the latest added
  // will be the first visible page in the stack
  gchar *background_name0 = g_strdup_printf (BACKGROUND_FRAME_NAME_TEMPLATE, 0);
  priv->current_background = g_object_new (GTK_TYPE_FRAME, "name", background_name0, NULL);
  gtk_widget_show (priv->current_background);
  gtk_container_add (GTK_CONTAINER (priv->background_stack), priv->current_background);
  g_free (background_name0);

  /* Dynamically set the base font-size based on the given window allocation. */
  priv->font_size_provider = gtk_css_provider_new ();

  priv->background_provider = gtk_css_provider_new ();
  // We start all the background frames transparent with no styling
  priv->current_background_css_props = g_strdup (TRANSPARENT_FRAME_CSS_PROPERTIES);
  gchar *background_css = g_strdup_printf(CSS_TEMPLATE,
                                          gtk_widget_get_name (priv->current_background),
                                          TRANSPARENT_FRAME_CSS_PROPERTIES,
                                          gtk_widget_get_name (priv->next_background),
                                          TRANSPARENT_FRAME_CSS_PROPERTIES);
  override_background_css (self, background_css);
  g_free (background_css);

  priv->main_area = eos_main_area_new ();
  gtk_overlay_add_overlay (GTK_OVERLAY (priv->overlay), priv->main_area);

  // We want the overlay to size to the main area, the widget on top. The
  // overlay gets its size request from the widget on the bottom, the
  // background frame with no minimum size. So we use a size group.
  priv->overlay_size_group = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);
  gtk_size_group_add_widget (priv->overlay_size_group, priv->background_stack);
  gtk_size_group_add_widget (priv->overlay_size_group, priv->main_area);

  priv->edge_finishing = gtk_drawing_area_new ();
  gtk_widget_set_vexpand (priv->edge_finishing, FALSE);
  gtk_widget_set_valign (priv->edge_finishing, GTK_ALIGN_START);
  /* has_window == FALSE is necessary for not catching input events */
  gtk_widget_set_has_window (priv->edge_finishing, FALSE);
  gtk_widget_set_size_request (priv->edge_finishing,
                               -1, _EOS_TOP_BAR_EDGE_FINISHING_HEIGHT_PX);
  g_signal_connect_after (priv->edge_finishing, "realize",
                          G_CALLBACK (after_edge_finishing_realize_cb), NULL);
  g_signal_connect (priv->edge_finishing, "draw",
                    G_CALLBACK (on_edge_finishing_draw_cb), NULL);
  gtk_overlay_add_overlay (GTK_OVERLAY (priv->overlay),
                           priv->edge_finishing);

  gtk_window_maximize (GTK_WINDOW (self));
  gtk_window_set_default_size (GTK_WINDOW (self), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

  g_signal_connect (priv->top_bar, "minimize-clicked",
                    G_CALLBACK (on_minimize_clicked_cb), self);
  g_signal_connect (priv->top_bar, "maximize-clicked",
                    G_CALLBACK (on_maximize_clicked_cb), self);
  g_signal_connect (priv->top_bar, "close-clicked",
                    G_CALLBACK (on_close_clicked_cb), self);
  g_signal_connect (self, "window-state-event",
                    G_CALLBACK (on_window_state_event_cb), NULL);

  eos_window_set_page_manager (self,
                               EOS_PAGE_MANAGER (eos_page_manager_new ()));
  // Make our internal widgets visible, so user needs only call show on the window.
  gtk_widget_show_all (priv->overlay);
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
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  return priv->page_manager;
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

  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosMainArea *main_area = EOS_MAIN_AREA (priv->main_area);

  priv->page_manager = page_manager;

  eos_main_area_set_content (main_area,
                             GTK_WIDGET (priv->page_manager));

  update_page (self);

  g_signal_connect_swapped (priv->page_manager, "notify::visible-page",
                            G_CALLBACK (update_page), self);
}

/**
 * eos_window_get_font_scaling_active:
 * @self: the window
 *
 * See #EosWindow:font-scaling-active for details.
 *
 * Returns: whether or not the font will automatically scale.
 */
gboolean
eos_window_get_font_scaling_active (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), FALSE);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  return priv->font_scaling_active;
}

/**
 * eos_window_set_font_scaling_active:
 * @self: the window
 * @is_scaling: true for enabling font scaling and 
 * false for disabling font scaling
 *
 * Sets whether or not the font will automatically scale.
 * See #EosWindow:font-scaling-active for details.
 */
void
eos_window_set_font_scaling_active (EosWindow *self,
                                    gboolean is_scaling)
{
  g_return_if_fail (self != NULL && EOS_IS_WINDOW (self));
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  priv->font_scaling_active = is_scaling;
}

/**
 * eos_window_get_font_scaling_default_size:
 * @self: the window
 *
 * See #EosWindow:font-scaling-default-size for details.
 *
 * Returns: the default font size by which the font size of children widgets
 * will scale.
 */
gint
eos_window_get_font_scaling_default_size (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), -1);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  return priv->font_scaling_default_size;
}

/**
 * eos_window_set_font_scaling_default_size:
 * @self: the window
 * @new_default_font_size: the new default font size
 *
 * Sets the default font size by which the font size of children widgets
 * will scale. See #EosWindow:font-scaling-default-size for details.
 */
void
eos_window_set_font_scaling_default_size (EosWindow *self,
                                          gint new_default_font_size)
{
  g_return_if_fail (self != NULL && EOS_IS_WINDOW (self));
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  priv->font_scaling_default_size = new_default_font_size;
}

/**
 * eos_window_get_font_scaling_default_window_size:
 * @self: the window
 *
 * See #EosWindow:font-scaling-default-window-size for details.
 *
 * Returns: the default window size by which font scaling
 * will occur.
 */
gint
eos_window_get_font_scaling_default_window_size (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), -1);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  return priv->font_scaling_default_window_size;
}

/**
 * eos_window_set_font_scaling_default_window_size:
 * @self: the window
 * @new_default_window_size: the new default window size
 *
 * Sets the default window size by which the font size of children widgets
 * will scale. See #EosWindow:font-scaling-default-window-size for details.
 */
void
eos_window_set_font_scaling_default_window_size (EosWindow *self,
                                                 gint new_default_window_size)
{
  g_return_if_fail (self != NULL && EOS_IS_WINDOW (self));
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  priv->font_scaling_default_window_size = new_default_window_size;
}

/**
 * eos_window_get_font_scaling_min_font_size:
 * @self: the window
 *
 * See #EosWindow:font-scaling-min-font-size for details.
 *
 * Returns: the minimum font size below which font scaling
 * won't occur.
 */
gint
eos_window_get_font_scaling_min_font_size (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), -1);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  return priv->font_scaling_min_font_size;
}

/**
 * eos_window_set_font_scaling_min_font_size:
 * @self: the window
 * @new_min_font_size: the new min font size
 *
 * Sets the min font size by which the font size of children widgets
 * will scale. See #EosWindow:font-scaling-min-font-size for
 * details.
 */
void 
eos_window_set_font_scaling_min_font_size (EosWindow *self,
                                           gint new_min_font_size)
{
  g_return_if_fail (self != NULL && EOS_IS_WINDOW (self));
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  priv->font_scaling_min_font_size = new_min_font_size;
}

/**
 * eos_window_get_font_scaling_calculated_font_size:
 * @self: the window
 *
 * See #EosWindow:font-scaling-calculated-font-size for details.
 *
 * Returns: the calculated font size by which the font size of children widgets
 * will scale.
 */
gdouble
eos_window_get_font_scaling_calculated_font_size (EosWindow *self)
{
  g_return_val_if_fail (self != NULL && EOS_IS_WINDOW (self), -1);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  return priv->font_scaling_calculated_font_size;
}
