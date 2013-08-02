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
 * The action buttons area contain "minimize" and "close" buttons.
 */
#define _EOS_STYLE_CLASS_TOP_BAR "top-bar"
#define _EOS_TOP_BAR_HEIGHT_PX 36
#define _EOS_TOP_BAR_BUTTON_PADDING_PX 4
#define _EOS_TOP_BAR_ICON_SIZE_PX 16
#define _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX 7
#define _EOS_TOP_BAR_BUTTON_SEPARATION_PX 8
#define _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX 6
#define _EOS_TOP_BAR_MINIMIZE_ICON_NAME "window-minimize-symbolic"
#define _EOS_TOP_BAR_CLOSE_ICON_NAME "window-close-symbolic"

G_DEFINE_TYPE (EosTopBar, eos_top_bar, GTK_TYPE_EVENT_BOX)

#define TOP_BAR_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_TOP_BAR, EosTopBarPrivate))

struct _EosTopBarPrivate
{
  GtkWidget *actions_grid;
  GtkWidget *left_top_bar_attach;
  GtkWidget *center_top_bar_attach;

  GtkWidget *left_top_bar_widget;
  GtkWidget *center_top_bar_widget;

  GtkWidget *minimize_button;
  GtkWidget *minimize_icon;
  GtkWidget *close_button;
  GtkWidget *close_icon;
};

enum {
  CLOSE_CLICKED,
  MINIMIZE_CLICKED,
  LAST_SIGNAL
};

static guint top_bar_signals[LAST_SIGNAL] = { 0 };

static void
eos_top_bar_get_preferred_height (GtkWidget *widget,
                                  int *minimum,
                                  int *natural)
{
  if (minimum != NULL)
    *minimum = _EOS_TOP_BAR_HEIGHT_PX;
  if (natural != NULL)
    *natural = _EOS_TOP_BAR_HEIGHT_PX;
}

/* Draw the edge finishing on the two lines inside the topbar; see
after_draw_cb() in eoswindow.c for the two lines outside the topbar */
static gboolean
eos_top_bar_draw (GtkWidget *self_widget,
                  cairo_t   *cr)
{
  GTK_WIDGET_CLASS (eos_top_bar_parent_class)->draw (self_widget, cr);

  gint width = gtk_widget_get_allocated_width (self_widget);
  cairo_set_line_width (cr, 1.0);
  /* Highlight: #ffffff, opacity 5% */
  cairo_set_source_rgba (cr, 1.0, 1.0, 1.0, 0.05);
  cairo_move_to (cr, 0, _EOS_TOP_BAR_HEIGHT_PX - 1.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);
  /* Baseline: #0a0a0a, opacity 100% */
  cairo_set_source_rgb (cr, 0.039, 0.039, 0.039);
  cairo_move_to (cr, 0, _EOS_TOP_BAR_HEIGHT_PX - 0.5);
  cairo_rel_line_to (cr, width, 0);
  cairo_stroke (cr);

  return GDK_EVENT_PROPAGATE;
}

static void
eos_top_bar_class_init (EosTopBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosTopBarPrivate));

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
   * Emitted when the close button has been activated.
   */
  top_bar_signals[CLOSE_CLICKED] =
      g_signal_new ("close-clicked",
                    G_OBJECT_CLASS_TYPE (object_class),
                    G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                    0,
                    NULL, NULL, NULL,
                    G_TYPE_NONE, 0);
}

static void
on_minimize_clicked_cb (GtkButton *button,
                        gpointer user_data)
{
  EosTopBar *self = EOS_TOP_BAR (user_data);
  g_signal_emit (self, top_bar_signals[MINIMIZE_CLICKED], 0);
}

static void
on_close_clicked_cb (GtkButton *button,
                     gpointer user_data)
{
  EosTopBar *self = EOS_TOP_BAR (user_data);
  g_signal_emit (self, top_bar_signals[CLOSE_CLICKED], 0);
}

static void
eos_top_bar_init (EosTopBar *self)
{
  GtkStyleContext *context;

  self->priv = TOP_BAR_PRIVATE (self);

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_TOP_BAR);

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);

  self->priv->actions_grid =
    g_object_new (GTK_TYPE_GRID,
                  "orientation", GTK_ORIENTATION_HORIZONTAL,
                  "hexpand", TRUE,
                  "halign", GTK_ALIGN_FILL,
                  "column-spacing", _EOS_TOP_BAR_BUTTON_SEPARATION_PX,
                  "margin-top", _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX,
                  "margin-bottom", _EOS_TOP_BAR_VERTICAL_BUTTON_MARGIN_PX,
                  "margin-left", _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX,
                  "margin-right", _EOS_TOP_BAR_HORIZONTAL_BUTTON_MARGIN_PX,
                  NULL);

  self->priv->left_top_bar_attach = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  self->priv->center_top_bar_attach = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_widget_set_hexpand (self->priv->center_top_bar_attach, TRUE);
  gtk_widget_set_halign (self->priv->center_top_bar_attach, GTK_ALIGN_CENTER);

  /* TODO implement adding actions and widgets to the actions_grid */

  self->priv->minimize_button =
    g_object_new (GTK_TYPE_BUTTON,
                  "halign", GTK_ALIGN_END,
                  "valign", GTK_ALIGN_CENTER,
                  NULL);
  self->priv->minimize_icon =
    gtk_image_new_from_icon_name (_EOS_TOP_BAR_MINIMIZE_ICON_NAME,
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  g_object_set(self->priv->minimize_icon,
               "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
               "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
               NULL);
  gtk_container_add (GTK_CONTAINER (self->priv->minimize_button),
                     self->priv->minimize_icon);

  self->priv->close_button =
    g_object_new (GTK_TYPE_BUTTON,
                  "halign", GTK_ALIGN_END,
                  "valign", GTK_ALIGN_CENTER,
                  NULL);
  self->priv->close_icon =
      gtk_image_new_from_icon_name (_EOS_TOP_BAR_CLOSE_ICON_NAME,
                                    GTK_ICON_SIZE_SMALL_TOOLBAR);
  g_object_set(self->priv->close_icon,
               "pixel-size", _EOS_TOP_BAR_ICON_SIZE_PX,
               "margin", _EOS_TOP_BAR_BUTTON_PADDING_PX,
               NULL);
  gtk_container_add (GTK_CONTAINER (self->priv->close_button),
                     self->priv->close_icon);

  gtk_container_add (GTK_CONTAINER (self->priv->actions_grid),
                     self->priv->left_top_bar_attach);
  gtk_container_add (GTK_CONTAINER (self->priv->actions_grid),
                     self->priv->center_top_bar_attach);
  gtk_container_add (GTK_CONTAINER (self->priv->actions_grid),
                     self->priv->minimize_button);
  gtk_container_add (GTK_CONTAINER (self->priv->actions_grid),
                     self->priv->close_button);

  gtk_container_add (GTK_CONTAINER (self), self->priv->actions_grid);

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);
  gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_FILL);

  g_signal_connect (self->priv->minimize_button, "clicked",
                    G_CALLBACK (on_minimize_clicked_cb), self);
  g_signal_connect (self->priv->close_button, "clicked",
                    G_CALLBACK (on_close_clicked_cb), self);
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

  EosTopBarPrivate *priv = self->priv;

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

  EosTopBarPrivate *priv = self->priv;

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
