/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eoswindow.h"

#include "eosapplication.h"
#include "eostopbar.h"

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
 */

G_DEFINE_TYPE (EosWindow, eos_window, GTK_TYPE_APPLICATION_WINDOW)

#define WINDOW_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_WINDOW, EosWindowPrivate))

struct _EosWindowPrivate
{
  EosApplication *application;

  GtkWidget *top_bar;
};

enum
{
  PROP_0,
  PROP_APPLICATION,
  NPROPS
};

static GParamSpec *eos_window_props[NPROPS] = { NULL, };

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

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
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
      self->priv->application = g_value_get_object (value);
      gtk_window_set_application (GTK_WINDOW (self),
                                  GTK_APPLICATION (self->priv->application));
      if (self->priv->application == NULL)
        g_error ("In order to create a window, you must have an application "
                 "for it to connect to.");
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
  unsigned border_width;

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
    {
      border_width = gtk_container_get_border_width (GTK_CONTAINER (self));
      child_allocation.x += border_width;
      child_allocation.y += border_width;
      child_allocation.width -= 2 * border_width;
      child_allocation.height -= 2 * border_width;
      child_allocation.width = MAX(1, child_allocation.width);
      child_allocation.height = MAX(1, child_allocation.height);
      gtk_widget_size_allocate (child, &child_allocation);
    }
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

  g_object_class_install_properties (object_class, NPROPS, eos_window_props);
}

static void on_minimize_clicked_cb (GtkWidget* top_bar, gpointer user_data)
{
  if (user_data != NULL)
    {
      gtk_widget_hide (GTK_WIDGET (user_data));
    }
}

static void on_close_clicked_cb (GtkWidget* top_bar, gpointer user_data)
{
  if (user_data != NULL)
    {
      gtk_widget_destroy (GTK_WIDGET (user_data));
    }
}

static void
eos_window_init (EosWindow *self)
{
  self->priv = WINDOW_PRIVATE (self);

  /* TODO work in progress */
  self->priv->top_bar = eos_top_bar_new ();
  gtk_widget_set_parent (self->priv->top_bar, GTK_WIDGET (self));

  g_signal_connect_swapped (default_screen, "monitors-changed",
                            G_CALLBACK (eos_window_resize_to_fullscreen), self);
  g_signal_connect_swapped (default_screen, "size-changed",
                            G_CALLBACK (eos_window_resize_to_fullscreen), self);
  g_signal_connect (self, "configure-event",
                    G_CALLBACK (eos_window_resize_to_fullscreen), NULL);

  gtk_window_set_decorated (GTK_WINDOW (self), FALSE);
  gtk_window_maximize (GTK_WINDOW (self));

  g_signal_connect (self->priv->top_bar, "minimize-clicked",
                    G_CALLBACK (on_minimize_clicked_cb), self);
  g_signal_connect (self->priv->top_bar, "close-clicked",
                    G_CALLBACK (on_close_clicked_cb), self);
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
