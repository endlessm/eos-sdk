/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eostopbar-private.h"

#include <glib-object.h>
#include <gtk/gtk.h>

#define _EOS_STYLE_CLASS_TOP_BAR "top-bar"
#define _EOS_TOP_BAR_HEIGHT_PX 32

G_DEFINE_TYPE (EosTopBar, eos_top_bar, GTK_TYPE_EVENT_BOX)

#define TOP_BAR_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_TOP_BAR, EosTopBarPrivate))

struct _EosTopBarPrivate
{
  GtkWidget *inner_grid;

  GtkWidget *actions_hbox;

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

static void
eos_top_bar_class_init (EosTopBarClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosTopBarPrivate));

  widget_class->get_preferred_height = eos_top_bar_get_preferred_height;

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

  self->priv->actions_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand (self->priv->actions_hbox, TRUE);
  gtk_widget_set_halign (self->priv->actions_hbox, GTK_ALIGN_START);

  /* TODO implement adding actions and widgets to the actions_hbox */

  self->priv->minimize_button = gtk_button_new ();
  gtk_widget_set_hexpand (self->priv->minimize_button, FALSE);
  gtk_widget_set_halign (self->priv->minimize_button, GTK_ALIGN_END);
  self->priv->minimize_icon =
    gtk_image_new_from_icon_name ("list-remove-symbolic",
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image (GTK_BUTTON (self->priv->minimize_button),
                        self->priv->minimize_icon);

  self->priv->close_button = gtk_button_new ();
  gtk_widget_set_hexpand (self->priv->close_button, FALSE);
  gtk_widget_set_halign (self->priv->close_button, GTK_ALIGN_END);
  self->priv->close_icon =
      gtk_image_new_from_icon_name ("window-close-symbolic",
                                    GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image (GTK_BUTTON (self->priv->close_button),
                        self->priv->close_icon);

  self->priv->inner_grid = gtk_grid_new ();
  gtk_widget_set_hexpand (self->priv->inner_grid, TRUE);
  gtk_widget_set_halign (self->priv->inner_grid, GTK_ALIGN_FILL);

  gtk_grid_attach(GTK_GRID (self->priv->inner_grid),
                  self->priv->actions_hbox,
                  0, 0, 1, 1);
  gtk_grid_attach_next_to (GTK_GRID (self->priv->inner_grid),
                           self->priv->close_button, NULL,
                           GTK_POS_RIGHT, 1, 1);
  gtk_grid_attach_next_to (GTK_GRID (self->priv->inner_grid),
                           self->priv->minimize_button,
                           self->priv->close_button,
                           GTK_POS_LEFT, 1, 1);

  gtk_container_add (GTK_CONTAINER (self), self->priv->inner_grid);

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
