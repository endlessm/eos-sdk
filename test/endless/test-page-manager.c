#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define PAGE1_NAME "page1"
#define PAGE2_NAME "page2"
#define PAGE3_NAME "page3"
#define PAGE1_PROP_STRING "prop1"
#define PAGE2_PROP_STRING "prop2"
#define PAGE3_PROP_STRING "prop3"
#define EXPECTED_PAGE_PROP_STRING PAGE2_PROP_STRING
#define EXPECTED_CHANGED_NAME "changed-name"
#define ADD_PAGE_MANAGER_TEST(path, test_func) \
  g_test_add ((path), PageManagerFixture, NULL, \
              pm_fixture_setup, (test_func), pm_fixture_teardown)
#define ADD_PAGE_MANAGER_TEST_WITH_ARGS(path, test_func, args) \
  g_test_add ((path), PageManagerFixture, args, \
              pm_fixture_setup, (test_func), pm_fixture_teardown)
#define BACKGROUND_SIZE_DEFAULT "100% 100%"
#define BACKGROUND_POSITION_DEFAULT "0% 0%"

typedef struct
{
  GtkWidget *pm;
  GtkWidget *page1;
  GtkWidget *page2;
  GtkWidget *page3;
} PageManagerFixture;

static void
pm_fixture_setup (PageManagerFixture *fixture,
                  gconstpointer       unused)
{
  fixture->pm = eos_page_manager_new ();
  fixture->page1 = gtk_label_new ("1");
  fixture->page2 = gtk_label_new ("2");
  fixture->page3 = gtk_label_new ("3");
  gtk_container_add_with_properties (GTK_CONTAINER (fixture->pm),
                                     fixture->page1,
                                     "name", PAGE1_NAME,
                                     "background-uri", PAGE1_PROP_STRING,
                                     "background-size", PAGE1_PROP_STRING,
                                     "background-position", PAGE1_PROP_STRING,
                                     NULL);
  gtk_container_add_with_properties (GTK_CONTAINER (fixture->pm),
                                     fixture->page2,
                                     "name", PAGE2_NAME,
                                     "background-uri", PAGE2_PROP_STRING,
                                     "background-size", PAGE2_PROP_STRING,
                                     "background-position", PAGE2_PROP_STRING,
                                     NULL);
  gtk_container_add_with_properties (GTK_CONTAINER (fixture->pm),
                                     fixture->page3,
                                     "name", PAGE3_NAME,
                                     "background-uri", PAGE3_PROP_STRING,
                                     "background-size", PAGE3_PROP_STRING,
                                     "background-position", PAGE3_PROP_STRING,
                                     "background-repeats", FALSE,
                                     NULL);
}

static void
pm_fixture_teardown (PageManagerFixture *fixture,
                     gconstpointer       unused)
{
  gtk_widget_destroy (fixture->pm);
}

static void
test_pm_get_set_background_repeats (PageManagerFixture *fixture,
                              gconstpointer unused)
{
  gboolean repeats;
  repeats = eos_page_manager_get_page_background_repeats (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page1);
  g_assert (repeats == TRUE);
  repeats = eos_page_manager_get_page_background_repeats (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page3);
  g_assert (repeats == FALSE);
  eos_page_manager_set_page_background_repeats (EOS_PAGE_MANAGER (fixture->pm),
                                     fixture->page3,
                                     TRUE);
  repeats = eos_page_manager_get_page_background_repeats (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page3);
  g_assert (repeats == TRUE);
}

static void
test_pm_child_prop_background_repeats (PageManagerFixture *fixture,
                                 gconstpointer unused)
{
  gboolean repeats;
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "background-repeats", &repeats,
                           NULL);
  g_assert (repeats == TRUE);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "background-repeats", &repeats,
                           NULL);
  g_assert (repeats == FALSE);
  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "background-repeats", TRUE,
                           NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "background-repeats", &repeats,
                           NULL);
  g_assert (repeats == TRUE);
}

static void
test_pm_child_prop_string (PageManagerFixture *fixture,
                           gconstpointer       data)
{
  gchar *prop_name = (gchar *)data;
  gchar *prop_string;
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           prop_name, &prop_string,
                           NULL);
  g_assert_cmpstr (prop_string, ==, PAGE1_PROP_STRING);
  g_free (prop_string);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           prop_name, &prop_string,
                           NULL);
  g_assert_cmpstr (prop_string, ==, PAGE2_PROP_STRING);
  g_free (prop_string);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           prop_name, &prop_string,
                           NULL);
  g_assert_cmpstr (prop_string, ==, PAGE3_PROP_STRING);
  g_free (prop_string);
  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page2,
                           prop_name, EXPECTED_CHANGED_NAME,
                           NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           prop_name, &prop_string,
                           NULL);
  g_assert_cmpstr (prop_string, ==, EXPECTED_CHANGED_NAME);
  g_free (prop_string);
}

static void
test_pm_no_background_uri (PageManagerFixture *fixture,
                            gconstpointer       unused)
{
  const gchar *background_uri_get;
  gchar *background_uri_prop;
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  background_uri_get = eos_page_manager_get_page_background_uri (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_uri_get, ==, NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), new_page,
                           "background-uri", &background_uri_prop,
                           NULL);
  g_assert_cmpstr (background_uri_prop, ==, NULL);
  g_free (background_uri_prop);
}

static void
test_pm_get_set_background_uri (PageManagerFixture *fixture,
                                gconstpointer       unused)
{
  const gchar *background_uri_get;
  const gchar *background_uri_name_1 = "first background uri name";
  const gchar *background_uri_name_2 = "second background uri name";
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  eos_page_manager_set_page_background_uri (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_uri_name_1);
  background_uri_get = eos_page_manager_get_page_background_uri (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_uri_get, ==, background_uri_name_1);

  eos_page_manager_set_page_background_uri (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_uri_name_2);
  background_uri_get = eos_page_manager_get_page_background_uri (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_uri_get, ==, background_uri_name_2);
}

static void
test_pm_default_background_size (PageManagerFixture *fixture,
                            gconstpointer       unused)
{
  const gchar *background_size_get;
  gchar *background_size_prop;
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  background_size_get = eos_page_manager_get_page_background_size (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_size_get, ==, BACKGROUND_SIZE_DEFAULT);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), new_page,
                           "background-size", &background_size_prop,
                           NULL);
  g_assert_cmpstr (background_size_prop, ==, BACKGROUND_SIZE_DEFAULT);
  g_free (background_size_prop);
}

static void
test_pm_get_set_background_size (PageManagerFixture *fixture,
                                gconstpointer       unused)
{
  const gchar *background_size_get;
  const gchar *background_size_name_1 = "first background size name";
  const gchar *background_size_name_2 = "second background size name";
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  eos_page_manager_set_page_background_size (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_size_name_1);
  background_size_get = eos_page_manager_get_page_background_size (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_size_get, ==, background_size_name_1);

  eos_page_manager_set_page_background_size (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_size_name_2);
  background_size_get = eos_page_manager_get_page_background_size (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_size_get, ==, background_size_name_2);
}

static void
test_pm_default_background_position (PageManagerFixture *fixture,
                            gconstpointer       unused)
{
  const gchar *background_position_get;
  gchar *background_position_prop;
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  background_position_get = eos_page_manager_get_page_background_position (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_position_get, ==, BACKGROUND_POSITION_DEFAULT);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), new_page,
                           "background-position", &background_position_prop,
                           NULL);
  g_assert_cmpstr (background_position_prop, ==, BACKGROUND_POSITION_DEFAULT);
  g_free (background_position_prop);
}

static void
test_pm_get_set_background_position (PageManagerFixture *fixture,
                                gconstpointer       unused)
{
  const gchar *background_position_get;
  const gchar *background_position_name_1 = "first background position name";
  const gchar *background_position_name_2 = "second background position name";
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  eos_page_manager_set_page_background_position (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_position_name_1);
  background_position_get = eos_page_manager_get_page_background_position (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_position_get, ==, background_position_name_1);

  eos_page_manager_set_page_background_position (EOS_PAGE_MANAGER (fixture->pm),
                                            new_page,
                                            background_position_name_2);
  background_position_get = eos_page_manager_get_page_background_position (EOS_PAGE_MANAGER (fixture->pm),
                                                             new_page);
  g_assert_cmpstr (background_position_get, ==, background_position_name_2);
}

static void
test_pm_remove_page_by_name (PageManagerFixture *fixture,
                             gconstpointer       unused)
{
  GList *pages = gtk_container_get_children (GTK_CONTAINER (fixture->pm));
  guint length = g_list_length (pages);
  g_list_free (pages);

  eos_page_manager_remove_page_by_name (EOS_PAGE_MANAGER (fixture->pm),
                                        PAGE2_NAME);
  pages = gtk_container_get_children (GTK_CONTAINER (fixture->pm));
  g_assert_cmpuint (g_list_length (pages), ==, length - 1);
  g_assert (g_list_find (pages, fixture->page1) != NULL);
  g_assert (g_list_find (pages, fixture->page2) == NULL);
  g_assert (g_list_find (pages, fixture->page3) != NULL);
  g_list_free (pages);
}

void
add_page_manager_tests (void)
{
  ADD_PAGE_MANAGER_TEST_WITH_ARGS ("/page-manager/child-prop-background-uri",
                                   test_pm_child_prop_string,
                                   "background-uri");
  ADD_PAGE_MANAGER_TEST ("/page-manager/no-background-uri",
                         test_pm_no_background_uri);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-background-uri",
                         test_pm_get_set_background_uri);
  ADD_PAGE_MANAGER_TEST_WITH_ARGS ("/page-manager/child-prop-background-size",
                                   test_pm_child_prop_string,
                                   "background-size");
  ADD_PAGE_MANAGER_TEST ("/page-manager/default-background-size",
                         test_pm_default_background_size);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-background-size",
                         test_pm_get_set_background_size);
  ADD_PAGE_MANAGER_TEST_WITH_ARGS ("/page-manager/child-prop-background-position",
                                   test_pm_child_prop_string,
                                   "background-position");
  ADD_PAGE_MANAGER_TEST ("/page-manager/default-background-position",
                         test_pm_default_background_position);
  ADD_PAGE_MANAGER_TEST ("/page-manager/set-background-position",
                         test_pm_get_set_background_position);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-background-repeats",
                         test_pm_get_set_background_repeats);
  ADD_PAGE_MANAGER_TEST ("/page-manager/child-prop-background-repeats",
                         test_pm_child_prop_background_repeats);
  ADD_PAGE_MANAGER_TEST ("/page-manager/remove-page-by-name",
                         test_pm_remove_page_by_name);
}
