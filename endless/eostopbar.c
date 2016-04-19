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
 * The #EosTopBar has two different areas that can be managed through this
 * class: a left widget and a center widget, which can both contain any other
 * widget.
 */
#define _EOS_TOP_BAR_HEIGHT_PX 36

typedef struct {
  GtkWidget *center_top_bar_attach;  /* needed to suppress default title */
  GtkWidget *left_top_bar_widget;
  GtkWidget *center_top_bar_widget;

  GtkWidget *credits_button;

  gboolean show_credits_button;
  guint credits_enter_handler;
  guint credits_leave_handler;
} EosTopBarPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosTopBar, eos_top_bar, GTK_TYPE_HEADER_BAR)

enum {
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
eos_top_bar_constructed (GObject *object)
{
  EosTopBar *self = EOS_TOP_BAR (object);
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);
  gtk_header_bar_set_custom_title (GTK_HEADER_BAR (self),
                                   priv->center_top_bar_attach);
}

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
  gboolean left_widget_visible = FALSE, center_widget_visible = FALSE;
  EosTopBar *self = EOS_TOP_BAR (widget);
  EosTopBarPrivate *priv = eos_top_bar_get_instance_private (self);
  if (priv->left_top_bar_widget)
    {
      left_widget_visible = gtk_widget_get_visible (priv->left_top_bar_widget);
      gtk_widget_set_visible (priv->left_top_bar_widget, TRUE);
    }
  if (priv->center_top_bar_widget)
    {
      center_widget_visible = gtk_widget_get_visible (priv->center_top_bar_widget);
      gtk_widget_set_visible (priv->center_top_bar_widget, TRUE);
    }

  GTK_WIDGET_CLASS (eos_top_bar_parent_class)->get_preferred_height (widget,
                                                                     minimum,
                                                                     natural);
  if (minimum != NULL)
    *minimum = MAX (_EOS_TOP_BAR_HEIGHT_PX, *minimum);
  if (natural != NULL)
    *natural = MAX (_EOS_TOP_BAR_HEIGHT_PX, *natural);

  if (priv->left_top_bar_widget)
    {
      gtk_widget_set_visible (priv->left_top_bar_widget, left_widget_visible);
    }
  if (priv->center_top_bar_widget)
    {
      gtk_widget_set_visible (priv->center_top_bar_widget, center_widget_visible);
    }
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
on_credits_clicked (GtkButton *button,
                    EosTopBar *self)
{
  g_signal_emit (self, top_bar_signals[CREDITS_CLICKED], 0);
}

static void
eos_top_bar_class_init (EosTopBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->constructed = eos_top_bar_constructed;
  object_class->get_property = eos_top_bar_get_property;
  object_class->set_property = eos_top_bar_set_property;
  widget_class->get_preferred_height = eos_top_bar_get_preferred_height;
  widget_class->draw = eos_top_bar_draw;

  gtk_widget_class_set_template_from_resource (widget_class,
                                               "/com/endlessm/sdk/widgets/topbar.ui");
  gtk_widget_class_bind_template_child_internal_private (widget_class,
                                                         EosTopBar,
                                                         center_top_bar_attach);
  gtk_widget_class_bind_template_child_internal_private (widget_class,
                                                         EosTopBar,
                                                         credits_button);
  gtk_widget_class_bind_template_callback (widget_class, on_credits_clicked);

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
eos_top_bar_init (EosTopBar *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
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
    {
      gtk_container_remove (GTK_CONTAINER (self), priv->left_top_bar_widget);
    }

  priv->left_top_bar_widget = left_top_bar_widget;
  if (left_top_bar_widget)
    {
      gtk_widget_set_valign (priv->left_top_bar_widget, GTK_ALIGN_CENTER);
      gtk_header_bar_pack_start (GTK_HEADER_BAR (self),
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
      gtk_widget_show (priv->center_top_bar_widget);
    }
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
  gtk_widget_set_visible (priv->credits_button, show_credits_button);
  g_object_notify_by_pspec (G_OBJECT (self),
                            eos_top_bar_props[PROP_SHOW_CREDITS_BUTTON]);
}
