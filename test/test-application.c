/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <inttypes.h> /* For PRIi64 */
#include <sys/stat.h> /* For file mode constants */
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define EXPECTED_TWO_WINDOW_ERRMSG "*You should not add more than one application window*"
#define EXPECTED_CONFIG_NOT_WRITABLE_ERRMSG "*Your user config directory*is not writable*"

#define APPLICATION_TEST_ID_BASE "com.endlessm.eosapplication.test"

typedef struct
{
  gchar *unique_id;
  EosApplication *app;
} ConfigDirFixture;

static void
test_two_windows (EosApplication *app)
{
  GtkWidget *win1 = eos_window_new (app);

  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GtkWidget *win2 = eos_window_new (app);
      gtk_widget_destroy (win2);
      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_TWO_WINDOW_ERRMSG);

  gtk_widget_destroy (win1);
}

static gchar *
generate_unique_app_id (void)
{
  return g_strdup_printf ("%s%" PRIi64,
                          APPLICATION_TEST_ID_BASE,
                          g_get_real_time ());
}

static void
config_dir_setup (ConfigDirFixture *fixture,
                  gconstpointer     unused)
{
  fixture->unique_id = generate_unique_app_id ();
  fixture->app = eos_application_new (fixture->unique_id,
                                      G_APPLICATION_FLAGS_NONE);
}

static void
config_dir_teardown (ConfigDirFixture *fixture,
                     gconstpointer     unused)
{
  /* Clean up the temporary config directory */
  GFile *config_dir = eos_application_get_config_dir (fixture->app);
  g_assert (g_file_delete (config_dir, NULL, NULL));

  g_free (fixture->unique_id);
  g_object_unref (fixture->app);
}

static void
test_config_dir_get (ConfigDirFixture *fixture,
                     gconstpointer     unused)
{
  GFile *dir1 = eos_application_get_config_dir (fixture->app);
  GFile *dir2;
  g_object_get (fixture->app, "config-dir", &dir2, NULL);

  g_assert (dir1 != NULL);
  g_assert (G_IS_FILE (dir1));
  g_assert (dir1 == dir2);

  g_object_unref (dir2);
}

static void
test_config_dir_returns_expected_path (ConfigDirFixture *fixture,
                                       gconstpointer     unused)
{
  GFile *config_dir = eos_application_get_config_dir (fixture->app);

  char *basename = g_file_get_basename (config_dir);
  g_assert_cmpstr (basename, ==, fixture->unique_id);
  g_free (basename);

  GFile *parent = g_file_get_parent (config_dir);
  char *dirname = g_file_get_path (parent);
  g_object_unref (parent);
  g_assert_cmpstr (dirname, ==, g_get_user_config_dir ());
  g_free (dirname);
}

static void
test_config_dir_exists (ConfigDirFixture *fixture,
                        gconstpointer     unused)
{
  GFile *config_dir = eos_application_get_config_dir (fixture->app);
  g_assert (g_file_query_exists (config_dir, NULL));
}

/* Helper function */
static void
set_writable (GFile   *file,
              gboolean writable)
{
  guint32 unwritable_mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP;
  guint32 writable_mode = unwritable_mode | S_IWUSR | S_IWGRP;

  g_assert (g_file_set_attribute_uint32 (file,
                                         G_FILE_ATTRIBUTE_UNIX_MODE,
                                         writable? writable_mode : unwritable_mode,
                                         G_FILE_QUERY_INFO_NONE,
                                         NULL, NULL));
}

static void
test_config_dir_fails_if_not_writable (ConfigDirFixture *fixture,
                                       gconstpointer unused)
{
  /* Pre-create the config dir and make it non-writable */
  char *config_path = g_build_filename (g_get_user_config_dir (),
                                        fixture->unique_id,
                                        NULL);
  GFile *precreated_config_dir = g_file_new_for_path (config_path);
  g_free (config_path);
  g_assert (g_file_make_directory (precreated_config_dir, NULL, NULL));

  set_writable (precreated_config_dir, FALSE);

  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GFile *config_dir = eos_application_get_config_dir (fixture->app);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_CONFIG_NOT_WRITABLE_ERRMSG);

  set_writable (precreated_config_dir, TRUE);

  g_object_unref (precreated_config_dir);
}

void
add_application_tests (void)
{
  ADD_APP_WINDOW_TEST ("/application/two-windows", test_two_windows);
  g_test_add ("/application/config-dir-get", ConfigDirFixture, NULL,
              config_dir_setup,
              test_config_dir_get,
              config_dir_teardown);
  g_test_add ("/application/config-dir-expected-path", ConfigDirFixture, NULL,
              config_dir_setup,
              test_config_dir_returns_expected_path,
              config_dir_teardown);
  g_test_add ("/application/config-dir-exists", ConfigDirFixture, NULL,
              config_dir_setup,
              test_config_dir_exists,
              config_dir_teardown);
  g_test_add ("/application/config-dir-fails-if-not-writable", ConfigDirFixture,
              NULL,
              config_dir_setup,
              test_config_dir_fails_if_not_writable,
              config_dir_teardown);
}
