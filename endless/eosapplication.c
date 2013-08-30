/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosapplication.h"

#include <gtk/gtk.h>

#include "eoswindow.h"

#define CSS_THEME_URI "resource:///com/endlessm/sdk/css/endless-widgets.css"

/**
 * SECTION:application
 * @short_description: Start here with your application
 * @title: Applications
 *
 * The #EosApplication class is where you start when programming your
 * application.
 * You should create a class that extends #EosApplication.
 *
 * You also need to think up an application ID.
 * This takes the form of a reverse domain name, and it should be unique.
 * This ID is used to make sure that only one copy of your application is
 * running at any time; if a user tries to start a second copy, then the first
 * copy is brought to the front.
 *
 * To set up your application's data and window, override the
 * #GApplication::startup function, like this example do-nothing application,
 * <quote>Smoke Grinder</quote>:
 * |[
 * const Lang = imports.lang;
 * const Endless = imports.gi.Endless;
 *
 * const SmokeGrinder = new Lang.Class ({
 *     Name: 'SmokeGrinder',
 *     Extends: Endless.Application,
 *
 *     vfunc_startup: function() {
 *         this.parent();
 *         this._window = new Endless.Window({application: this});
 *         this._window.show_all();
 *     },
 * });
 *
 * let app = new SmokeGrinder({ application_id: "com.example.smokegrinder",
 *                              flags: 0 });
 * app.run(ARGV);
 * ]|
 */

G_DEFINE_TYPE (EosApplication, eos_application, GTK_TYPE_APPLICATION)

#define APPLICATION_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_APPLICATION, EosApplicationPrivate))

struct _EosApplicationPrivate
{
  EosWindow *main_application_window;
};

static void
eos_application_activate (GApplication *application)
{
  EosApplication *self = EOS_APPLICATION (application);

  G_APPLICATION_CLASS (eos_application_parent_class)->activate (application);

  /* Raise the main application window if it is iconified. This behavior will
  be default in GTK at some future point, in which case the following
  paragraph can be removed. */
  if (self->priv->main_application_window)
    {
      gtk_window_present (GTK_WINDOW (self->priv->main_application_window));
    }

  /* TODO: Should it be required to override activate() as in GApplication? */
}

static void
eos_application_startup (GApplication *application)
{
  G_APPLICATION_CLASS (eos_application_parent_class)->startup (application);

  GtkCssProvider *provider = gtk_css_provider_new ();

  /* Reset CSS for SDK applications and apply our own theme on top of it. This
  is so that we do not interfere with existing, complicated Adwaita theming.
  */
  GFile *css_file = g_file_new_for_uri (CSS_THEME_URI);
  gtk_css_provider_load_from_file (provider, css_file, NULL);
  g_object_unref (css_file);

  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_SETTINGS);
  g_debug ("Initialized theme\n");

  g_object_unref (provider);
}

static void
eos_application_window_added (GtkApplication *application,
                              GtkWindow *window)
{
  EosApplication *self = EOS_APPLICATION (application);

  GTK_APPLICATION_CLASS (eos_application_parent_class)->window_added (
    application, window);

  /* If the new window is an EosWindow, then it is our main application window;
  it should be raised when the application is activated */
  if (EOS_IS_WINDOW (window))
    {
      if (self->priv->main_application_window != NULL)
        {
          g_error ("You should not add more than one application window.");
        }
      g_object_ref (window);
      self->priv->main_application_window = EOS_WINDOW (window);
    }
}

static void
eos_application_window_removed (GtkApplication *application,
                                GtkWindow *window)
{
  EosApplication *self = EOS_APPLICATION (application);

  GTK_APPLICATION_CLASS (eos_application_parent_class)->window_removed (
    application, window);

  if (EOS_IS_WINDOW (window))
    {
      if (self->priv->main_application_window == NULL)
        {
          g_warning ("EosWindow is being removed from EosApplication, although "
                     "none was added.");
          return;
        }
      if (self->priv->main_application_window != EOS_WINDOW (window))
        g_warning ("A different EosWindow is being removed from EosApplication "
                   "than the one that was added.");
      g_object_unref (window);
      self->priv->main_application_window = NULL;
    }
}

static void
on_app_id_set (EosApplication *self)
{
  const gchar *id = g_application_get_application_id (G_APPLICATION (self));
  g_set_prgname (id);
}

static void
eos_application_class_init (EosApplicationClass *klass)
{
  GApplicationClass *g_application_class = G_APPLICATION_CLASS (klass);
  GtkApplicationClass *gtk_application_class = GTK_APPLICATION_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosApplicationPrivate));

  g_application_class->activate = eos_application_activate;
  g_application_class->startup = eos_application_startup;
  gtk_application_class->window_added = eos_application_window_added;
  gtk_application_class->window_removed = eos_application_window_removed;
}

static void
eos_application_init (EosApplication *self)
{
  self->priv = APPLICATION_PRIVATE (self);
  g_signal_connect (self, "notify::application-id",
                    G_CALLBACK (on_app_id_set), self);
}

/* Public API */

/**
 * eos_application_new:
 * @application_id: a unique identifier for the application, for example a
 * reverse domain name.
 * @flags: flags to apply to the application; see #GApplicationFlags.
 *
 * Create a new application. For the application ID, use a reverse domain name,
 * such as <code>com.endlessm.weather</code>. See g_application_id_is_valid()
 * for the full rules for application IDs.
 *
 * Returns: a pointer to the application.
 */
EosApplication *
eos_application_new (const gchar      *application_id,
                     GApplicationFlags flags)
{
  return g_object_new (EOS_TYPE_APPLICATION,
                       "application-id", application_id,
                       "flags", flags,
                       NULL);
}
