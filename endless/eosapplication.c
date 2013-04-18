/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosapplication.h"

#include <gtk/gtk.h>

#include "eoswindow.h"

#define DESKTOP_THEME_NAME "EndlessOS"
#define ICON_THEME_NAME DESKTOP_THEME_NAME
#define THEME_RELATIVE_PATH "themes" G_DIR_SEPARATOR_S \
                            DESKTOP_THEME_NAME G_DIR_SEPARATOR_S \
                            "gtk-3.0"
#define THEME_CSS_RELATIVE_PATH THEME_RELATIVE_PATH G_DIR_SEPARATOR_S "gtk.css"
#define THEME_RESOURCE_RELATIVE_PATH THEME_RELATIVE_PATH G_DIR_SEPARATOR_S \
                                     "gtk.gresource"

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
  const char * const *system_data_dirs = g_get_system_data_dirs ();
  GtkCssProvider *provider = gtk_css_provider_new ();
  const char * const *datadir;
  GtkSettings *default_settings;

  /* Make sure that Endless applications use the EndlessOS theme */
  /* FIXME: This is not necessary when the theme is complete and the system-wide
  theme is set to Endless. */
  for (datadir = system_data_dirs; *datadir != NULL; datadir++)
    {
      char *theme_path = g_build_filename (*datadir,
                                           THEME_CSS_RELATIVE_PATH,
                                           NULL);
      char *resource_path = g_build_filename (*datadir,
                                              THEME_RESOURCE_RELATIVE_PATH,
                                              NULL);
      GResource *resource;
      g_debug ("Searching %s for theme\n", *datadir);

      /* Ignore exceptions */
      if ((resource = g_resource_load (resource_path, NULL)) != NULL)
        {
          g_resources_register (resource);
          g_resource_unref (resource);
          g_free (resource_path);
          if (gtk_css_provider_load_from_path (provider, theme_path, NULL))
            {
              g_debug ("Using theme from %s\n", theme_path);
              g_free (theme_path);
              break;
            }
        }

      g_free (theme_path);
      g_free (resource_path);
    }

  gtk_style_context_add_provider_for_screen (gdk_screen_get_default (),
                                             GTK_STYLE_PROVIDER (provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_THEME);
  g_debug ("Initialized theme\n");

  g_object_unref (provider);

  /* Also make sure that Endless applications use the EndlessOS icon theme */
  default_settings = gtk_settings_get_default ();
  if (default_settings != NULL)
    {
      g_object_set (default_settings,
                    "gtk-icon-theme-name", ICON_THEME_NAME,
                    NULL);
      g_debug ("Initialized icon theme");
    }
  else
    g_warning ("Could not get default GtkSettings, icon theme not initialized");

  G_APPLICATION_CLASS (eos_application_parent_class)->startup (application);
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
