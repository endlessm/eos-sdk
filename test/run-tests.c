/* Copyright 2013 Endless Mobile, Inc. */

#include <inttypes.h> /* For PRIi64 */
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define APPLICATION_TEST_ID_BASE "com.endlessm.eosapplication.test"

/* App ID based on timestamp so that test applications don't collide */
gchar *
generate_unique_app_id (void)
{
  return g_strdup_printf ("%s%" PRIi64,
                          APPLICATION_TEST_ID_BASE,
                          g_get_real_time ());
}

/* Test fixture for running a test from an EosApplication's "startup" handler */
void
app_window_test_fixture_setup (AppWindowTestFixture *fixture,
                               gconstpointer callback)
{
  gchar *app_id = generate_unique_app_id ();
  fixture->app = eos_application_new (app_id, 0);
  g_free (app_id);
  g_signal_connect(fixture->app, "startup", G_CALLBACK (callback),
                   NULL);
}

void
app_window_test_fixture_test (AppWindowTestFixture *fixture,
                              gconstpointer unused)
{
  g_application_run (G_APPLICATION (fixture->app), 0, NULL);
}

void
app_window_test_fixture_teardown (AppWindowTestFixture *fixture,
                                  gconstpointer unused)
{
  g_object_unref (fixture->app);
}


static void
add_widget_to_list_cb (GtkWidget *widget,
                       gpointer   data)
{
  GList **list = (GList**) data;
  *list = g_list_append (*list, widget);
}

GList *
container_get_all_children (GtkContainer *container)
{
  GList *children = NULL;
  gtk_container_forall (container,
                        add_widget_to_list_cb,
                        &children);
  return children;
}

static GtkWidget *
container_find_descendant_with_type_recurse (GtkWidget *widget,
                                             GType type)
{
  if (G_TYPE_CHECK_INSTANCE_TYPE (widget, type))
    return widget;
  if (GTK_IS_CONTAINER (widget))
    {
      GList *children = container_get_all_children (GTK_CONTAINER (widget));
      for (guint i = 0; i < g_list_length (children); i++)
        {
          GtkWidget *descendant = container_find_descendant_with_type_recurse (g_list_nth_data (children, i),
                                                                               type);
          if (descendant != NULL)
            return descendant;
        }
    }
  return NULL;
}

/* Query all the descendants of container, return the first found of the desired
 type, or null*/
GtkWidget *
container_find_descendant_with_type (GtkContainer *container,
                                     GType type)
{
  return container_find_descendant_with_type_recurse (GTK_WIDGET (container), type);
}

int
main (int    argc,
      char **argv)
{
  g_test_init (&argc, &argv, NULL);
  gtk_init (&argc, &argv);

  add_init_tests ();
  add_hello_tests ();
  add_application_tests ();
  add_window_tests ();
  add_page_manager_tests ();
  add_splash_page_manager_tests ();
  add_action_menu_tests ();
  add_action_button_tests ();
  add_flexy_grid_test ();

  return g_test_run ();
}
