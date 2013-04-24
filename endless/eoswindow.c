/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eoswindow.h"

#include "eosapplication.h"

#include <gtk/gtk.h>

/**
 * SECTION:window
 * @short_description: A window for your application
 * @title: Window
 *
 * Stub
 */

G_DEFINE_TYPE (EosWindow, eos_window, GTK_TYPE_APPLICATION_WINDOW)

#define WINDOW_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_WINDOW, EosWindowPrivate))

struct _EosWindowPrivate
{
  EosApplication *application;
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
        g_critical ("In order to create a window, you must have an application "
                    "for it to connect to.");
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_window_class_init (EosWindowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosWindowPrivate));

  object_class->get_property = eos_window_get_property;
  object_class->set_property = eos_window_set_property;

  eos_window_props[PROP_APPLICATION] =
    g_param_spec_object ("application", "Application",
                         "Application associated with this window",
                         EOS_TYPE_APPLICATION,
                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS, eos_window_props);
}

static void
eos_window_init (EosWindow *self)
{
  self->priv = WINDOW_PRIVATE (self);
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
