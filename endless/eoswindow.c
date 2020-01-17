/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "string.h"
#include "endless.h"
#include "eostopbar-private.h"

#include <gtk/gtk.h>

#ifdef USE_METRICS
#include <eosmetrics/eosmetrics.h>
#endif

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
#define DEFAULT_WINDOW_HEIGHT 570

#define BACKGROUND_FRAME_NAME_TEMPLATE "_eos-window-background-%d"

#define FONT_SIZE_TEMPLATE "EosWindow { font-size: %spx; }"

#define TRANSPARENT_FRAME_CSS_PROPERTIES "{ background-image: none;\n" \
                                          " background-color: transparent\n;" \
                                          " border-width: 0px; }\n"

#define BACKGROUND_FRAME_CSS_PROPERTIES_TEMPLATE "{ background-image: url(\"%s\");\n" \
                                                  " background-size: %s;\n" \
                                                  " background-position: %s;\n" \
                                                  " background-repeat: %s;\n" \
                                                  " border-width: 0px; }\n"

#define CSS_TEMPLATE "#%s %s #%s %s"

#define UNMAXIMIZE_EVENT "2b5c044d-d819-4e2c-a3a6-c485c1ac371e"

typedef struct {
  EosApplication *application;

  GtkWidget *top_bar;
  GtkWidget *overlay;
  GtkSizeGroup *overlay_size_group;
  GtkWidget *current_background;
  GtkWidget *next_background;
  GtkWidget *background_stack;

  EosPageManager *page_manager;

  /* For scaling base font-size */
  GtkCssProvider *font_size_provider;
  gboolean font_scaling_active;
  gint font_scaling_default_size;
  gint font_scaling_default_window_size;
  gint font_scaling_min_font_size;
  gdouble font_scaling_calculated_font_size;

  /* For keeping track of what to display alongside the current page */
  GtkWidget *current_page;
  gulong visible_child_property_handler;
  GtkCssProvider *background_provider;
  gchar *current_background_css_props;

  /* Only send unmaximize metric once */
  gboolean has_been_unmaximized;
  guint unmaximize_timeout_id;

  gint64 last_configure;
  guint in_resize_id;
  gint width;
  gint height;
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
                                 gtk_stack_get_transition_type (GTK_STACK (pm)));
  gtk_stack_set_transition_duration (GTK_STACK (priv->background_stack),
                                     gtk_stack_get_transition_duration (GTK_STACK (pm)));
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
 * update_visible_child_properties:
 * @widget: the page
 * @child_property: the property that changed
 * @user_data: pointer to the window
 *
 * Updates the currently displaying page when one of its child properties
 * changes.
 */
static void
update_visible_child_properties (GtkWidget  *widget,
                                GParamSpec *child_property,
                                gpointer    data)
{
  EosWindow *self = (EosWindow *)data;
  const gchar *property_name = child_property->name;
  if (g_strcmp0 (property_name, "left-topbar-widget") == 0)
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
 * Ensures that the state of the window and top bar are in line with the
 * currently showing page and its child properties.
 */
static void
update_page (EosWindow *self)
{
  if (gtk_widget_in_destruction (GTK_WIDGET (self)))
    return;

  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  EosPageManager *pm = EOS_PAGE_MANAGER (priv->page_manager);

  if (priv->current_page)
    {
      g_signal_handler_disconnect (priv->current_page,
                                   priv->visible_child_property_handler);
    }
  priv->current_page = gtk_stack_get_visible_child (GTK_STACK (pm));

  sync_stack_animation (self);
  update_page_left_topbar (self);
  update_page_center_topbar (self);
  update_page_background (self);
  gtk_stack_set_transition_type (GTK_STACK (priv->background_stack),
                                 GTK_STACK_TRANSITION_TYPE_NONE);

  if (priv->current_page)
    {
      priv->visible_child_property_handler =
        g_signal_connect (priv->current_page,
                          "child-notify",
                          G_CALLBACK (update_visible_child_properties),
                          self);
    }
}

static void
on_image_credits_enabled_changed (GActionGroup *group,
                                  const gchar  *action_name,
                                  gboolean      enabled,
                                  EosWindow    *self)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  eos_top_bar_set_show_credits_button (EOS_TOP_BAR (priv->top_bar), enabled);
}

static void
eos_window_constructed (GObject *object)
{
  EosWindow *self = EOS_WINDOW (object);
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  G_OBJECT_CLASS (eos_window_parent_class)->constructed (object);

  GtkApplication *application =
    gtk_window_get_application (GTK_WINDOW (object));
  GFile *credits_file =
    eos_application_get_image_attribution_file (EOS_APPLICATION (application));
  eos_top_bar_set_show_credits_button (EOS_TOP_BAR (priv->top_bar),
                                       (credits_file != NULL));
  g_signal_connect (application, "action-enabled-changed::image-credits",
                    G_CALLBACK (on_image_credits_enabled_changed), self);
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
  if (priv->in_resize_id)
    g_source_remove (priv->in_resize_id);

  G_OBJECT_CLASS (eos_window_parent_class)->finalize (object);
}

static void
eos_window_font_scale (EosWindow *self,
                       gint       allocated_height)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  if (!priv->font_scaling_active)
    return;

  GtkStyleProvider *provider = GTK_STYLE_PROVIDER (priv->font_size_provider);
  gdouble new_size = priv->font_scaling_default_size *
    ((gdouble) allocated_height / priv->font_scaling_default_window_size);

  new_size = MAX (new_size, priv->font_scaling_min_font_size);

  if (new_size == priv->font_scaling_calculated_font_size)
    return;

  priv->font_scaling_calculated_font_size = new_size;

  /* A float will only have one decimal point when printed as a string. The
   * decimal point can be represented as a comma or period when using either
   * Imperial or metric units. However, the CSS parser only recognizes periods
   * as valid decimal points. Therefore, we convert the float to a string using
   * a period as the decimal point. */
  gchar font_size_float_str[G_ASCII_DTOSTR_BUF_SIZE];
  g_ascii_dtostr (font_size_float_str, G_ASCII_DTOSTR_BUF_SIZE,
                  priv->font_scaling_calculated_font_size);

  gchar *font_size_css = g_strdup_printf (FONT_SIZE_TEMPLATE,
                                          font_size_float_str);
  GdkScreen *screen = gdk_screen_get_default ();

  gtk_style_context_remove_provider_for_screen (screen, provider);
  gtk_css_provider_load_from_data (GTK_CSS_PROVIDER (provider), font_size_css,
                                   -1, NULL);  /* ignore error */
  gtk_style_context_add_provider_for_screen (screen, provider,
                                             GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  g_free (font_size_css);
}

/* Updates the base font size depending on the window size. */
static void
eos_window_size_allocate (GtkWidget *window, GtkAllocation *allocation)
{
  eos_window_font_scale (EOS_WINDOW (window), allocation->height);
  GTK_WIDGET_CLASS (eos_window_parent_class)->size_allocate (window, allocation);
}

static gboolean
in_resize_timeout (gpointer data)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (data);

  gtk_style_context_remove_class (gtk_widget_get_style_context (data),
                                  "in-resize");
  priv->in_resize_id = 0;

  return G_SOURCE_REMOVE;
}

static gboolean
eos_window_configure_event (GtkWidget *widget, GdkEventConfigure *event)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (EOS_WINDOW (widget));

  if (event->width != priv->width || event->height != priv->height)
    {
      guint64 current_time = g_source_get_time (g_main_current_source ());

      if (current_time - priv->last_configure < 500000)
        {
          if (priv->in_resize_id)
            g_source_remove (priv->in_resize_id);
          else
            gtk_style_context_add_class (gtk_widget_get_style_context (widget),
                                         "in-resize");

          priv->in_resize_id = g_timeout_add (500, in_resize_timeout, widget);
        }

      priv->width = event->width;
      priv->height = event->height;
      priv->last_configure = current_time;
    }

  return GTK_WIDGET_CLASS (eos_window_parent_class)->configure_event (widget, event);
}

static void
eos_window_class_init (EosWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = eos_window_constructed;
  object_class->get_property = eos_window_get_property;
  object_class->set_property = eos_window_set_property;
  object_class->finalize = eos_window_finalize;
  widget_class->size_allocate = eos_window_size_allocate;
  widget_class->configure_event = eos_window_configure_event;

  gtk_widget_class_set_css_name (widget_class, "EosWindow");

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
                      16,
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
                      10,
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
                      16,
                      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS, eos_window_props);
}

static void
update_screen (EosWindow *self)
{
  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (self));
  if (eos_is_composite_tv_screen (gtk_window_get_screen (GTK_WINDOW (self))))
    gtk_style_context_add_class (context, EOS_STYLE_CLASS_COMPOSITE);
  else
    gtk_style_context_remove_class (context, EOS_STYLE_CLASS_COMPOSITE);
}

#ifdef USE_METRICS

static gboolean
record_unmaximize_metric (EosWindow *self)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  GtkApplication *app = gtk_window_get_application (GTK_WINDOW (self));
  const char *app_id = g_application_get_application_id (G_APPLICATION (app));
  EmtrEventRecorder *recorder = emtr_event_recorder_get_default ();
  emtr_event_recorder_record_event (recorder, UNMAXIMIZE_EVENT,
                                    g_variant_new_string (app_id));
  priv->has_been_unmaximized = TRUE;
  priv->unmaximize_timeout_id = 0;
  return G_SOURCE_REMOVE;
}

static void
on_maximize_state_change (EosWindow  *self,
                          GParamSpec *pspec)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);
  if (priv->has_been_unmaximized)
    return;

  /* We assume that if the window remains unmaximized for 10 seconds, then the
   * user meant to unmaximize it on purpose and is using the app that way. This
   * also bypasses any initial uncertainty from the window manager on whether
   * the window was supposed to be maximized or not. */
  if (gtk_window_is_maximized (GTK_WINDOW (self)))
    {
      if (priv->unmaximize_timeout_id != 0)
        {
          g_source_remove (priv->unmaximize_timeout_id);
          priv->unmaximize_timeout_id = 0;
        }
    }
  else
    {
      priv->unmaximize_timeout_id =
        g_timeout_add_seconds (10, (GSourceFunc) record_unmaximize_metric,
                               self);
    }
}

#endif /* USE_METRICS */

static void
on_credits_clicked (GtkWidget *top_bar,
                    EosWindow *self)
{
  GtkApplication *application = gtk_window_get_application (GTK_WINDOW (self));
  /* application cannot be NULL */
  g_action_group_activate_action (G_ACTION_GROUP (application), "image-credits",
                                  NULL);
}

static void
eos_window_init (EosWindow *self)
{
  EosWindowPrivate *priv = eos_window_get_instance_private (self);

  update_screen (self);

  priv->top_bar = eos_top_bar_new ();
  gtk_widget_show_all (priv->top_bar);
  gtk_window_set_titlebar (GTK_WINDOW (self), priv->top_bar);

  priv->overlay = gtk_overlay_new ();
  gtk_style_context_add_class (gtk_widget_get_style_context (priv->overlay),
                               EOS_WINDOW_STYLE_CLASS_INNER);
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

  // We want the overlay to size to the page manager, the widget on top. The
  // overlay gets its size request from the widget on the bottom, the
  // background frame with no minimum size. So we use a size group.
  priv->overlay_size_group = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);
  gtk_size_group_add_widget (priv->overlay_size_group, priv->background_stack);

  gtk_window_maximize (GTK_WINDOW (self));
  gtk_window_set_default_size (GTK_WINDOW (self), DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);

  g_signal_connect (priv->top_bar, "credits-clicked",
                    G_CALLBACK (on_credits_clicked), self);
  g_signal_connect (self, "notify::screen", G_CALLBACK (update_screen), NULL);
#ifdef USE_METRICS
  g_signal_connect (self, "notify::is-maximized",
                    G_CALLBACK(on_maximize_state_change), NULL);
#endif

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

  if (priv->page_manager != NULL)
    {
      gtk_size_group_remove_widget (priv->overlay_size_group,
                                    GTK_WIDGET (priv->page_manager));
      gtk_container_remove (GTK_CONTAINER (priv->overlay),
                            GTK_WIDGET (priv->page_manager));
    }
  priv->page_manager = page_manager;
  gtk_overlay_add_overlay (GTK_OVERLAY (priv->overlay),
                           GTK_WIDGET (priv->page_manager));
  gtk_size_group_add_widget (priv->overlay_size_group,
                             GTK_WIDGET (priv->page_manager));

  update_page (self);

  g_signal_connect_swapped (priv->page_manager, "notify::visible-child",
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
