/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eostopbar-private.h"

#include <glib-object.h>
#include <gtk/gtk.h>

/*
 * SECTION:topbar
 * @short_description: The top bar of the window, above the main area of your
 * application.
 * @title: TopBar
 * 
 * The #EosTopBar has three different areas that can be managed through this 
 * class: a left widget, center widget, and action buttons area.
 * 
 * The action buttons area contain "minimize", "maximize" and "close" buttons.
 */
#define _EOS_STYLE_CLASS_TOP_BAR "top-bar"
#define _EOS_STYLE_CLASS_UNMAXIMIZED "unmaximized"
#define _EOS_TOP_BAR_HEIGHT_PX 36
#define _EOS_TOP_BAR_BUTTON_PADDING_PX 4
#define _EOS_TOP_BAR_ICON_SIZE_PX 16
#define _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX 7
#define _EOS_TOP_BAR_BUTTON_SEPARATION_PX 8
#define _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX 6
#define _EOS_TOP_BAR_MINIMIZE_ICON_NAME "window-minimize-symbolic"
#define _EOS_TOP_BAR_MAXIMIZE_ICON_NAME "window-maximize-symbolic"
#define _EOS_TOP_BAR_UNMAXIMIZE_ICON_NAME "window-restore-symbolic"
#define _EOS_TOP_BAR_CLOSE_ICON_NAME "window-close-symbolic"
#define _EOS_TOP_BAR_CREDITS_ICON_NAME "user-info-symbolic"

typedef struct {
  GtkWidget *actions_grid;
  GtkWidget *left_top_bar_attach;
  GtkWidget *center_top_bar_attach;

  GtkWidget *left_top_bar_widget;
  GtkWidget *center_top_bar_widget;

  GtkWidget *minimize_button;
  GtkWidget *minimize_icon;
  GtkWidget *maximize_button;
  GtkWidget *maximize_icon;
  GtkWidget *close_button;
  GtkWidget *close_icon;
  GtkWidget *credits_button;
  GtkWidget *credits_icon;
  GtkWidget *credits_stack;

  gboolean show_credits_button;
  guint credits_enter_handler;
  guint credits_leave_handler;
} EosTopBarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosTopBar, eos_top_bar, GTK_TYPE_EVENT_BOX)

enum {
  CLOSE_CLICKED,
  MINIMIZE_CLICKED,
  MAXIMIZE_CLICKED,
  CREDITS_CLICKED,
  LAST_SIGNAL
};

static guint top_bar_signals[LAST_SIGNAL] = { 0 };

enum {
  PROP_0,
  PROP_SHOW_CREDITS_BUTTON,
  NPROPS
};

static GParamSpec *eos_top_bar_props[NPROPS] = { NULL, };

static void
eos_top_bar_get_property (GObject    *object,
                          guint       property_id,
                          GValue     *value,
                          GParamSpec *pspec)
{
  EosTopBar *self = EOS_TOP_BAR (object);

  switch (property_id)
    {
    case PROP_SHOW_CREDITS_BUTTON:
      g_value_set_boolean (value, eos_top_bar_get_show_credits_button (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_top_bar_set_property (GObject      *object,
                          guint         property_id,
                          const GValue *value,
                          GParamSpec   *pspec)
{
  EosTopBar *self = EOS_TOP_BAR (object);

  switch (property_id)
    {
    case PROP_SHOW_CREDITS_BUTTON:
      eos_top_bar_set_show_credits_button (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_top_bar_get_preferred_height (GtkWidget *widget,
                                  int *minimum,
                                  int *natural)
{
  EosTopBar *self = EOS_TOP_BAR (widget);
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);
  gboolean left_widget_visible = gtk_widget_get_visible (priv->left_top_bar_widget);
  gboolean center_widget_visible = gtk_widget_get_visible (priv->center_top_bar_widget);
  gtk_widget_set_visible (priv->left_top_bar_widget, TRUE);
  gtk_widget_set_visible (priv->center_top_bar_widget, TRUE);

  GTK_WIDGET_CLASS (eos_top_bar_parent_class)->get_preferred_height (widget,
                                                                     minimum,
                                                                     natural);
  if (minimum != NULL)
    *minimum = MAX (_EOS_TOP_BAR_HEIGHT_PX, *minimum);
  if (natural != NULL)
    *natural = MAX (_EOS_TOP_BAR_HEIGHT_PX, *natural);

  gtk_widget_set_visible (priv->left_top_bar_widget, left_widget_visible);
  gtk_widget_set_visible (priv->center_top_bar_widget, center_widget_visible);
}

/* Draw the edge finishing on the two lines inside the topbar; see
after_draw_cb() in eoswindow.c for the two lines outside the topbar */
static gboolean
eos_top_bar_draw (GtkWidget *self_widget,
                  cairo_t   *cr)
{
  GTK_WIDGET_CLASS (eos_top_bar_parent_class)->draw (self_widget, cr);

  gint width = gtk_widget_get_allocated_width (self_widget);
  gint height = gtk_widget_get_allocated_height (self_widget);
  cairo_set_line_width (cr, 1.0);
  /* Highlight: #ffffff, opacity 5% */
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.05);
  cairo_move_to (cr, 0, height - 1.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);
  /* Baseline: #0a0a0a, opacity 100% */
  cairo_set_source_rgb (cr, 0.039, 0.039, 0.039);
  cairo_move_to (cr, 0, height - 0.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);

  return GDK_EVENT_PROPAGATE;
}

static void
eos_top_bar_class_init (EosTopBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->get_property = eos_top_bar_get_property;
  object_class->set_property = eos_top_bar_set_property;
  widget_class->get_preferred_height = eos_top_bar_get_preferred_height;
  widget_class->draw = eos_top_bar_draw;

  /*
   * Emitted when the minimize button has been activated.
   */
  top_bar_signals[MINIMIZE_CLICKED] =
      g_signal_new ("minimize-clicked",
                    G_OBJECT_CLASS_TYPE (object_class),
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                    0,
                    NULL, NULL, NULL,
                    G_TYPE_NONE, 0);

  /*
   * Emitted when the maximize button has been activated.
   */
  top_bar_signals[MAXIMIZE_CLICKED] =
      g_signal_new ("maximize-clicked",
                    G_OBJECT_CLASS_TYPE (object_class),
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                    0,
                    NULL, NULL, NULL,
                    G_TYPE_NONE, 0);

  /*
   * Emitted when the close button has been activated.
   */
  top_bar_signals[CLOSE_CLICKED] =
      g_signal_new ("close-clicked",
                    G_OBJECT_CLASS_TYPE (object_class),
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                    0,
                    NULL, NULL, NULL,
                    G_TYPE_NONE, 0);

  top_bar_signals[CREDITS_CLICKED] =
    g_signal_new ("credits-clicked", G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION, 0, NULL, NULL, NULL,
                  G_TYPE_NONE, 0);

  eos_top_bar_props[PROP_SHOW_CREDITS_BUTTON] =
    g_param_spec_boolean ("show-credits-button", "Show credits button",
                         "Whether the credits button is discoverable",
                         FALSE,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS, eos_top_bar_props);
}

static void
on_minimize_clicked_cb (GtkButton *button,
                        gpointer user_data)
{
  EosTopBar *self = EOS_TOP_BAR (user_data);
  g_signal_emit (self, top_bar_signals[MINIMIZE_CLICKED], 0);
}

static void
on_maximize_clicked_cb (GtkButton *button,
                        gpointer   user_data)
{
  EosTopBar *self = EOS_TOP_BAR (user_data);
  g_signal_emit (self, top_bar_signals[MAXIMIZE_CLICKED], 0);
}

static void
on_close_clicked_cb (GtkButton *button,
                     gpointer user_data)
{
  EosTopBar *self = EOS_TOP_BAR (user_data);
  g_signal_emit (self, top_bar_signals[CLOSE_CLICKED], 0);
}

static gboolean
on_stack_hover (GtkStack *stack,
                GdkEvent *event,
                gpointer  data)
{
  gboolean show = GPOINTER_TO_INT (data);
  gtk_stack_set_visible_child_name (stack, show ? "button" : "blank");
  return GDK_EVENT_PROPAGATE;
}

static void
on_credits_clicked (GtkButton *button,
                    EosTopBar *self)
{
  g_signal_emit (self, top_bar_signals[CREDITS_CLICKED], 0);
}

static void
eos_top_bar_init (EosTopBar *self)
{
  GtkStyleContext *context;
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_TOP_BAR);

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);

  priv->actions_grid =
    g_object_new (GTK_TYPE_GRID,
                  "orientation", GTK_ORIENTATION_HORIZONTAL,
                  "hexpand", TRUE,
                  "halign", GTK_ALIGN_FILL,
                  "column-spacing", _EOS_TOP_BAR_BUTTON_SEPARATION_PX,
                  "margin-top", _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX,
                  "margin-bottom", _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX,
                  "margin-start", _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX,
                  "margin-end", _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX,
                  NULL);

  priv->left_top_bar_attach = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  priv->center_top_bar_attach = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_widget_set_hexpand (priv->center_top_bar_attach, TRUE);
  gtk_widget_set_halign (priv->center_top_bar_attach, GTK_ALIGN_CENTER);
  gtk_widget_set_vexpand (priv->left_top_bar_attach, TRUE);
  gtk_widget_set_vexpand (priv->center_top_bar_attach, TRUE);

  /* TODO implement adding actions and widgets to the actions_grid */

  priv->minimize_button =
    g_object_new (GTK_TYPE_BUTTON,
                  "can-focus", FALSE,
                  "halign", GTK_ALIGN_END,
                  "valign", GTK_ALIGN_CENTER,
                  NULL);
  priv->minimize_icon =
    gtk_image_new_from_icon_name (_EOS_TOP_BAR_MINIMIZE_ICON_NAME,
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  g_object_set(priv->minimize_icon,
               "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
               "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
               NULL);
  gtk_container_add (GTK_CONTAINER (priv->minimize_button),
                     priv->minimize_icon);

  priv->maximize_button =
    g_object_new (GTK_TYPE_BUTTON,
                  "can-focus", FALSE,
                  "halign", GTK_ALIGN_END,
                  "valign", GTK_ALIGN_CENTER,
                  NULL);
  priv->maximize_icon = gtk_image_new ();
  eos_top_bar_update_window_maximized (self, TRUE);
  g_object_set(priv->maximize_icon,
               "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
               "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
               NULL);
  gtk_container_add (GTK_CONTAINER (priv->maximize_button),
                     priv->maximize_icon);

  priv->close_button =
    g_object_new (GTK_TYPE_BUTTON,
                  "can-focus", FALSE,
                  "halign", GTK_ALIGN_END,
                  "valign", GTK_ALIGN_CENTER,
                  NULL);
  priv->close_icon =
      gtk_image_new_from_icon_name (_EOS_TOP_BAR_CLOSE_ICON_NAME,
                                    GTK_ICON_SIZE_SMALL_TOOLBAR);
  g_object_set(priv->close_icon,
               "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
               "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
               NULL);
  gtk_container_add (GTK_CONTAINER (priv->close_button),
                     priv->close_icon);

  /* This works like a revealer but it's really a GtkStack so that it takes up
  space and presents a target even when it's not shown. */
  priv->credits_stack = gtk_stack_new ();
  gtk_stack_set_transition_type (GTK_STACK (priv->credits_stack),
                                 GTK_STACK_TRANSITION_TYPE_CROSSFADE);
  gtk_widget_add_events (priv->credits_stack,
                         GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
  gtk_stack_add_named (GTK_STACK (priv->credits_stack),
                       gtk_event_box_new (), "blank");
  priv->credits_button = g_object_new (GTK_TYPE_BUTTON,
                                       "can-focus", FALSE,
                                       "halign", GTK_ALIGN_END,
                                       "valign", GTK_ALIGN_CENTER,
                                       NULL);
  priv->credits_icon =
    gtk_image_new_from_icon_name (_EOS_TOP_BAR_CREDITS_ICON_NAME,
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  g_object_set (priv->credits_icon,
                "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
                "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
                NULL);
  gtk_container_add (GTK_CONTAINER (priv->credits_button), priv->credits_icon);
  gtk_stack_add_named (GTK_STACK (priv->credits_stack),
                       priv->credits_button, "button");

  gtk_container_add (GTK_CONTAINER (priv->actions_grid),
                     priv->left_top_bar_attach);
  gtk_container_add (GTK_CONTAINER (priv->actions_grid),
                     priv->center_top_bar_attach);
  gtk_container_add (GTK_CONTAINER (priv->actions_grid), priv->credits_stack);
  gtk_container_add (GTK_CONTAINER (priv->actions_grid),
                     priv->minimize_button);
  gtk_container_add (GTK_CONTAINER (priv->actions_grid),
                     priv->maximize_button);
  gtk_container_add (GTK_CONTAINER (priv->actions_grid),
                     priv->close_button);

  gtk_container_add (GTK_CONTAINER (self), priv->actions_grid);

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);
  gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_FILL);

  g_signal_connect (priv->minimize_button, "clicked",
                    G_CALLBACK (on_minimize_clicked_cb), self);
  g_signal_connect (priv->maximize_button, "clicked",
                    G_CALLBACK (on_maximize_clicked_cb), self);
  g_signal_connect (priv->close_button, "clicked",
                    G_CALLBACK (on_close_clicked_cb), self);
  priv->credits_enter_handler =
    g_signal_connect (priv->credits_stack, "enter-notify-event",
                      G_CALLBACK (on_stack_hover), GINT_TO_POINTER (TRUE));
  priv->credits_leave_handler =
    g_signal_connect (priv->credits_stack, "leave-notify-event",
                      G_CALLBACK (on_stack_hover), GINT_TO_POINTER (FALSE));
  g_signal_handler_block (priv->credits_stack, priv->credits_enter_handler);
  g_signal_handler_block (priv->credits_stack, priv->credits_leave_handler);
  g_signal_connect (priv->credits_button, "clicked",
                    G_CALLBACK (on_credits_clicked), self);
}

GtkWidget *
eos_top_bar_new (void)
{
  return GTK_WIDGET (g_object_new (EOS_TYPE_TOP_BAR, NULL));
}

/*
 * eos_top_bar_set_left_widget:
 * @self: the top bar
 * @left_top_bar_widget: the left top bar widget to be set
 * 
 * Sets the left widget in the top bar.
 */
void
eos_top_bar_set_left_widget (EosTopBar *self,
                             GtkWidget *left_top_bar_widget)
{
  g_return_if_fail (EOS_IS_TOP_BAR (self));
  g_return_if_fail (left_top_bar_widget == NULL || GTK_IS_WIDGET (left_top_bar_widget));

  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);

  if (priv->left_top_bar_widget == left_top_bar_widget)
    return;

  if (priv->left_top_bar_widget)
    gtk_container_remove (GTK_CONTAINER (priv->left_top_bar_attach),
                       priv->left_top_bar_widget);

  priv->left_top_bar_widget = left_top_bar_widget;
  if (left_top_bar_widget)
    {
      gtk_container_add (GTK_CONTAINER (priv->left_top_bar_attach),
                         priv->left_top_bar_widget);
    }
}

/*
 * eos_top_bar_set_center_widget:
 * @self: the top bar
 * @center_top_bar_widget: the center top bar widget to be set
 * 
 * Sets the center widget in the top bar.
 */
void
eos_top_bar_set_center_widget (EosTopBar *self,
                               GtkWidget *center_top_bar_widget)
{
  g_return_if_fail (EOS_IS_TOP_BAR (self));
  g_return_if_fail (center_top_bar_widget == NULL || GTK_IS_WIDGET (center_top_bar_widget));

  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);

  if (priv->center_top_bar_widget == center_top_bar_widget)
    return;

  if (priv->center_top_bar_widget)
    gtk_container_remove (GTK_CONTAINER (priv->center_top_bar_attach),
                          priv->center_top_bar_widget);

  priv->center_top_bar_widget = center_top_bar_widget;
  if (center_top_bar_widget)
    {
      gtk_container_add (GTK_CONTAINER (priv->center_top_bar_attach),
                         priv->center_top_bar_widget);
    }
}

/*
 * eos_top_bar_update_window_maximized:
 * @self: the top bar
 * @is_maximized: whether the window is currently maximized
 *
 * Private method for eos_window to update the topbar on the window maximized
 * state. The top bar will flip the asset of the maximized button depending on
 * the state
 */
void
eos_top_bar_update_window_maximized (EosTopBar *self,
                                     gboolean is_maximized)
{
  g_return_if_fail (EOS_IS_TOP_BAR (self));
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);

  gchar *icon_name = is_maximized ? _EOS_TOP_BAR_UNMAXIMIZE_ICON_NAME : _EOS_TOP_BAR_MAXIMIZE_ICON_NAME;
  gtk_image_set_from_icon_name (GTK_IMAGE (priv->maximize_icon),
                                icon_name,
                                GTK_ICON_SIZE_SMALL_TOOLBAR);

  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (self));
  if (!is_maximized)
    gtk_style_context_add_class (context, _EOS_STYLE_CLASS_UNMAXIMIZED);
  else
    gtk_style_context_remove_class (context, _EOS_STYLE_CLASS_UNMAXIMIZED);
}

/*
 * eos_top_bar_get_show_credits_button:
 * @self: the top bar
 *
 * See eos_top_bar_set_show_credits_button().
 *
 * Returns: %TRUE if credits button should be discoverable, %FALSE if not.
 */
gboolean
eos_top_bar_get_show_credits_button (EosTopBar *self)
{
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);
  return priv->show_credits_button;
}

/*
 * eos_top_bar_set_show_credits_button:
 * @self: the top bar
 * @show_credits_button: whether the credits button should be discoverable.
 *
 * Gets whether the credits button should be discoverable.
 * Note that the credits button is not visible as such, but when the mouse
 * hovers over it, it becomes visible if this is set to %TRUE.
 * If this is %FALSE, the button never becomes visible.
 */
void
eos_top_bar_set_show_credits_button (EosTopBar *self,
                                     gboolean   show_credits_button)
{
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);
  if (priv->show_credits_button == show_credits_button)
    return;

  priv->show_credits_button = show_credits_button;
  if (show_credits_button)
    {
      g_signal_handler_unblock (priv->credits_stack,
                                priv->credits_enter_handler);
      g_signal_handler_unblock (priv->credits_stack,
                                priv->credits_leave_handler);
    }
  else
    {
      gtk_stack_set_visible_child_name (GTK_STACK (priv->credits_stack),
                                        "blank");
      g_signal_handler_block (priv->credits_stack, priv->credits_enter_handler);
      g_signal_handler_block (priv->credits_stack, priv->credits_leave_handler);
    }
  g_object_notify_by_pspec (G_OBJECT (self),
                            eos_top_bar_props[PROP_SHOW_CREDITS_BUTTON]);
}
