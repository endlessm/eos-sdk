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
  GOnce  init_config_dir_once;
  GFile *config_dir;

  EosWindow *main_application_window;
};

enum
{
  PROP_0,
  PROP_CONFIG_DIR,
  NPROPS
};

static GParamSpec *eos_application_props[NPROPS] = { NULL, };

static void
eos_application_get_property (GObject    *object,
                              guint       property_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  EosApplication *self = EOS_APPLICATION (object);

  switch (property_id)
    {
    case PROP_CONFIG_DIR:
      g_value_set_object (value, eos_application_get_config_dir (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_application_finalize (GObject *object)
{
  EosApplication *self = EOS_APPLICATION (object);
  g_clear_object (&self->priv->config_dir);

  G_OBJECT_CLASS (eos_application_parent_class)->finalize (object);
}

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

static gpointer
ensure_config_dir_exists_and_is_writable (EosApplication *self)
{
  const gchar *xdg_path = g_get_user_config_dir ();
  const gchar *app_id = g_application_get_application_id (G_APPLICATION (self));
  GFile *xdg_dir = g_file_new_for_path (xdg_path);
  GFile *config_dir = g_file_get_child (xdg_dir, app_id);
  gchar *config_path = g_file_get_path (config_dir); /* For error reporting */

  g_object_unref (xdg_dir);

  GError *error = NULL;
  if (!g_file_make_directory_with_parents (config_dir, NULL, &error))
    {
      if (error->domain == G_IO_ERROR && error->code == G_IO_ERROR_EXISTS)
        {
          g_clear_error (&error); /* Ignore G_IO_ERROR_EXISTS */
        }
      else
        {
          g_error ("There was an error creating the user config directory %s: "
                   "%s",
                   config_path, error->message);
        }
    }

  GFileInfo *info = g_file_query_info (config_dir,
                                       G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE,
                                       G_FILE_QUERY_INFO_NONE,
                                       NULL,
                                       &error);
  if (info == NULL)
    {
      g_error ("Checking the user config directory %s failed. This means "
               "something strange is going on in your home directory: %s",
               config_path, error->message);
    }
  if (!g_file_info_get_attribute_boolean(info,
                                         G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE))
    {
      g_error ("Your user config directory %s is not writable. This means "
               "something strange is going on in your home directory.",
               config_path);
    }

  g_object_unref (info);
  g_free (config_path);

  self->priv->config_dir = config_dir;
  return NULL;
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

  EosApplication *self = EOS_APPLICATION (application);
  g_once (&self->priv->init_config_dir_once,
          (GThreadFunc)ensure_config_dir_exists_and_is_writable, self);
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

  /* Just in case, since g_set_prgname() does not always update the GDK
  program class, under mysterious circumstances */
  gchar *capitalized_id = g_strdup (id);
  if (capitalized_id != NULL && capitalized_id[0] != '\0')
    capitalized_id[0] = g_ascii_toupper (capitalized_id[0]);
  gdk_set_program_class (capitalized_id);
}

static void
eos_application_class_init (EosApplicationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GApplicationClass *g_application_class = G_APPLICATION_CLASS (klass);
  GtkApplicationClass *gtk_application_class = GTK_APPLICATION_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosApplicationPrivate));

  object_class->get_property = eos_application_get_property;
  object_class->finalize = eos_application_finalize;
  g_application_class->activate = eos_application_activate;
  g_application_class->startup = eos_application_startup;
  gtk_application_class->window_added = eos_application_window_added;
  gtk_application_class->window_removed = eos_application_window_removed;

  /**
   * EosApplication:config-dir:
   *
   * A directory appropriate for storing per-user configuration information for
   * this application.
   * Accessing this property guarantees that the directory exists and is
   * writable.
   * See also eos_application_get_config_dir() for more information.
   */
  eos_application_props[PROP_CONFIG_DIR] =
    g_param_spec_object ("config-dir", "Config dir",
                         "User configuration directory for this application",
                         G_TYPE_FILE,
                         G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS,
                                     eos_application_props);
}

static void
eos_application_init (EosApplication *self)
{
  self->priv = APPLICATION_PRIVATE (self);
  self->priv->init_config_dir_once = (GOnce)G_ONCE_INIT;
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

/**
 * eos_application_get_config_dir:
 * @self: the application
 *
 * Gets a #GFile pointing to the application-specific user configuration
 * directory.
 * This directory is located in <code>XDG_USER_CONFIG_DIR</code>, which usually
 * expands to <filename class="directory">~/.config</filename>.
 * The directory name is the same as the application's unique ID (see
 * #GApplication:application-id.)
 *
 * You should use this directory to store configuration data specific to your
 * application and specific to one user, such as cookies.
 *
 * Calling this function will also ensure that the directory exists and is
 * writable.
 * If it does not exist, it will be created.
 * If it cannot be created, or it exists but is not writable, the program will
 * abort.
 *
 * Returns: (transfer none): A #GFile pointing to the user config directory.
 */
GFile *
eos_application_get_config_dir (EosApplication *self)
{
  g_once (&self->priv->init_config_dir_once,
          (GThreadFunc)ensure_config_dir_exists_and_is_writable, self);
  return self->priv->config_dir;
}
