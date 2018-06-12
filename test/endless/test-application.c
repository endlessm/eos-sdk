/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <sys/stat.h> /* For file mode constants */
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define EXPECTED_TWO_WINDOW_ERRMSG "*You should not add more than one application window*"
#define EXPECTED_CONFIG_NOT_WRITABLE_ERRMSG "*Your user config directory*is not writable*"

typedef struct
{
  gchar *unique_id;
  EosApplication *app;
} UniqueAppFixture;

static void
test_two_windows (EosApplication *app)
{
  GtkWidget *win1 = eos_window_new (app);

  if (g_test_subprocess ())
    {
      GtkWidget *win2 = eos_window_new (app);
      gtk_widget_destroy (win2);
      return;
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_TWO_WINDOW_ERRMSG);

  gtk_widget_destroy (win1);
}

static void
unique_app_setup (UniqueAppFixture *fixture,
                  gconstpointer     unused)
{
  fixture->unique_id = generate_unique_app_id ();
  fixture->app = eos_application_new (fixture->unique_id,
                                      G_APPLICATION_FLAGS_NONE);
}

static void
unique_app_teardown (UniqueAppFixture *fixture,
                     gconstpointer     unused)
{
  /* Clean up the temporary config directory */
  GFile *config_dir = eos_application_get_config_dir (fixture->app);
  g_assert (g_file_delete (config_dir, NULL, NULL));

  g_free (fixture->unique_id);
  g_object_unref (fixture->app);
}

static void
test_config_dir_get (UniqueAppFixture *fixture,
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
test_image_attribution_file_get_set (UniqueAppFixture *fixture,
                                     gconstpointer     unused)
{
  GFile *file1, *file2;
  GFileIOStream *stream;
  g_object_get (fixture->app, "image-attribution-file", &file1, NULL);

  g_assert_null (file1);

  file1 = g_file_new_tmp (NULL, &stream, NULL);
  g_assert_nonnull (file1);
  g_io_stream_close (G_IO_STREAM (stream), NULL, NULL);
  g_object_unref (stream);
  g_object_set (fixture->app, "image-attribution-file", file1, NULL);
  g_object_get (fixture->app, "image-attribution-file", &file2, NULL);

  g_assert_true (g_file_equal (file1, file2));

  g_object_unref (file1);
  g_object_unref (file2);
}

static void
test_config_dir_returns_expected_path (UniqueAppFixture *fixture,
                                       gconstpointer     unused)
{
  GFile *config_dir = eos_application_get_config_dir (fixture->app);
  /* XDG_CONFIG_HOME may be a relative path, so resolve it via file handles */
  GFile *xdg_user_config_dir = g_file_new_for_path (g_get_user_config_dir ());

  char *basename = g_file_get_basename (config_dir);
  g_assert_cmpstr (basename, ==, fixture->unique_id);
  g_free (basename);

  GFile *parent = g_file_get_parent (config_dir);
  char *dirname = g_file_get_path (parent);
  g_object_unref (parent);

  char *xdg_dirname = g_file_get_path (xdg_user_config_dir);
  g_object_unref (xdg_user_config_dir);

  g_assert_cmpstr (dirname, ==, xdg_dirname);
  g_free (dirname);
  g_free (xdg_dirname);
}

static void
test_config_dir_exists (UniqueAppFixture *fixture,
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
test_config_dir_fails_if_not_writable (UniqueAppFixture *fixture,
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

  if (g_test_subprocess ())
    {
      GFile *config_dir = eos_application_get_config_dir (fixture->app);
      return;
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_CONFIG_NOT_WRITABLE_ERRMSG);

  set_writable (precreated_config_dir, TRUE);

  g_object_unref (precreated_config_dir);
}

void
add_application_tests (void)
{
  ADD_APP_WINDOW_TEST ("/application/two-windows", test_two_windows);

#define ADD_APP_TEST(path, func) \
  g_test_add((path), UniqueAppFixture, NULL, \
             unique_app_setup, (func), unique_app_teardown)

  ADD_APP_TEST ("/application/config-dir-get", test_config_dir_get);
  ADD_APP_TEST ("/application/image-attribution-file-get-set",
                test_image_attribution_file_get_set);
  ADD_APP_TEST ("/application/config-dir-expected-path",
                test_config_dir_returns_expected_path);
  ADD_APP_TEST ("/application/config-dir-exists", test_config_dir_exists);

  /* Only run this test if UID is not root; root can write to any directory no
  matter what its permissions. */
  if (getuid() > 0 && geteuid() > 0)
    {
      ADD_APP_TEST ("/application/config-dir-fails-if-not-writable",
                    test_config_dir_fails_if_not_writable);
    }

#undef ADD_APP_TEST
}
