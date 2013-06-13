#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define PAGE1_NAME "page1"
#define PAGE2_NAME "page2"
#define PAGE3_NAME "page3"
#define PAGE1_PROP_STRING "prop1"
#define PAGE2_PROP_STRING "prop2"
#define PAGE3_PROP_STRING "prop3"
#define EXPECTED_PAGE_NAME PAGE2_NAME
#define EXPECTED_CHANGED_PAGE_NAME "changed-name"
#define DUPLICATE_PAGE_NAME "duplicate-name"
#define EXPECTED_DUPLICATE_PAGE_NAME_ERRMSG "*Not setting page name to \"" \
  DUPLICATE_PAGE_NAME "\", because page manager already contains a page by " \
  "that name*"
#define EXPECTED_PAGE_PROP_STRING PAGE2_PROP_STRING
#define EXPECTED_CHANGED_NAME "changed-name"
#define ADD_PAGE_MANAGER_TEST(path, test_func) \
  g_test_add ((path), PageManagerFixture, NULL, \
              pm_fixture_setup, (test_func), pm_fixture_teardown)
#define ADD_PAGE_MANAGER_TEST_WITH_ARGS(path, test_func, args) \
  g_test_add ((path), PageManagerFixture, args, \
              pm_fixture_setup, (test_func), pm_fixture_teardown)
#define ADD_EMPTY_PAGE_MANAGER_TEST(path, test_func) \
  g_test_add ((path), PageManagerFixture, NULL, \
              empty_pm_fixture_setup, (test_func), pm_fixture_teardown);
#define DURATION_DEFAULT 200
#define DURATION_1 1
#define DURATION_2 9999
#define BACKGROUND_SIZE_DEFAULT "100% 100%"
#define BACKGROUND_POSITION_DEFAULT "0% 0%"

typedef struct
{
  GtkWidget *pm;
  GtkWidget *page1;
  GtkWidget *page2;
  GtkWidget *page3;
  GtkWidget *toolbox2;
} PageManagerFixture;

static void
pm_fixture_setup (PageManagerFixture *fixture,
                  gconstpointer       unused)
{
  fixture->pm = eos_page_manager_new ();
  fixture->page1 = gtk_label_new ("1");
  fixture->page2 = gtk_label_new ("2");
  fixture->page3 = gtk_label_new ("3");
  fixture->toolbox2 = gtk_label_new ("toolbox2");
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
                                     "custom-toolbox-widget", fixture->toolbox2,
                                     "background-uri", PAGE2_PROP_STRING,
                                     "background-size", PAGE2_PROP_STRING,
                                     "background-position", PAGE2_PROP_STRING,
                                     NULL);
  gtk_container_add_with_properties (GTK_CONTAINER (fixture->pm),
                                     fixture->page3,
                                     "name", PAGE3_NAME,
                                     "page-actions", TRUE,
                                     "background-uri", PAGE3_PROP_STRING,
                                     "background-size", PAGE3_PROP_STRING,
                                     "background-position", PAGE3_PROP_STRING,
                                     "background-repeats", FALSE,
                                     NULL);
}

static void
empty_pm_fixture_setup (PageManagerFixture *fixture,
                        gconstpointer       unused)
{
  fixture->pm = eos_page_manager_new ();
}

static void
pm_fixture_teardown (PageManagerFixture *fixture,
                     gconstpointer       unused)
{
  gtk_widget_destroy (fixture->pm);
}

static void
test_pm_get_set_visible_page (PageManagerFixture *fixture,
                              gconstpointer       unused)
{
  GtkWidget *visible_page;
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page != fixture->page2);
  eos_page_manager_set_visible_page (EOS_PAGE_MANAGER (fixture->pm),
                                     fixture->page2);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == fixture->page2);
}

static void
test_pm_prop_visible_page (PageManagerFixture *fixture,
                           gconstpointer       unused)
{
  GtkWidget *visible_page;
  g_object_get (fixture->pm, "visible-page", &visible_page, NULL);
  g_assert (visible_page != fixture->page2);
  g_object_set (fixture->pm, "visible-page", fixture->page2, NULL);
  g_object_get (fixture->pm, "visible-page", &visible_page, NULL);
  g_assert (visible_page == fixture->page2);
}

static void
test_pm_get_set_visible_page_name (PageManagerFixture *fixture,
                                   gconstpointer       unused)
{
  const gchar *name;
  name = eos_page_manager_get_visible_page_name (EOS_PAGE_MANAGER (fixture->pm));
  g_assert_cmpstr (name, !=, EXPECTED_PAGE_NAME);
  eos_page_manager_set_visible_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                          EXPECTED_PAGE_NAME);
  name = eos_page_manager_get_visible_page_name (EOS_PAGE_MANAGER (fixture->pm));
  g_assert_cmpstr (name, ==, EXPECTED_PAGE_NAME);
}

static void
test_pm_prop_visible_page_name (PageManagerFixture *fixture,
                                gconstpointer       unused)
{
  gchar *name;
  g_object_get (fixture->pm, "visible-page-name", &name, NULL);
  g_assert_cmpstr (name, !=, EXPECTED_PAGE_NAME);
  g_free (name);
  g_object_set (fixture->pm, "visible-page-name", EXPECTED_PAGE_NAME, NULL);
  g_object_get (fixture->pm, "visible-page-name", &name, NULL);
  g_assert_cmpstr (name, ==, EXPECTED_PAGE_NAME);
  g_free (name);
}

static void
test_pm_get_set_page_name (PageManagerFixture *fixture,
                           gconstpointer       unused)
{
  const gchar *name;
  name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                         fixture->page1);
  g_assert_cmpstr (name, ==, PAGE1_NAME);
  name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                         fixture->page2);
  g_assert_cmpstr (name, ==, PAGE2_NAME);
  name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                         fixture->page3);
  g_assert_cmpstr (name, ==, PAGE3_NAME);
  eos_page_manager_set_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                  fixture->page2,
                                  EXPECTED_CHANGED_NAME);
  name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                         fixture->page2);
  g_assert_cmpstr (name, ==, EXPECTED_CHANGED_NAME);
}

static void
test_pm_child_prop_name (PageManagerFixture *fixture,
                         gconstpointer       unused)
{
  gchar *name;
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "name", &name,
                           NULL);
  g_assert_cmpstr (name, ==, PAGE1_NAME);
  g_free (name);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "name", &name,
                           NULL);
  g_assert_cmpstr (name, ==, PAGE2_NAME);
  g_free (name);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "name", &name,
                           NULL);
  g_assert_cmpstr (name, ==, PAGE3_NAME);
  g_free (name);
  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "name", EXPECTED_CHANGED_NAME,
                           NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "name", &name,
                           NULL);
  g_assert_cmpstr (name, ==, EXPECTED_CHANGED_NAME);
  g_free (name);
}

static void
test_pm_get_set_page_actions (PageManagerFixture *fixture,
                              gconstpointer unused)
{
  gboolean actions;
  actions = eos_page_manager_get_page_actions (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page1);
  g_assert (actions == FALSE);
  actions = eos_page_manager_get_page_actions (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page3);
  g_assert (actions == TRUE);
  eos_page_manager_set_page_actions (EOS_PAGE_MANAGER (fixture->pm),
                                     fixture->page3,
                                     FALSE);
  actions = eos_page_manager_get_page_actions (EOS_PAGE_MANAGER (fixture->pm),
                                               fixture->page3);
  g_assert (actions == FALSE);
}

static void
test_pm_child_prop_page_actions (PageManagerFixture *fixture,
                                 gconstpointer unused)
{
  gboolean actions;
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "page-actions", &actions,
                           NULL);
  g_assert (actions == FALSE);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "page-actions", &actions,
                           NULL);
  g_assert (actions == TRUE);
  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "page-actions", FALSE,
                           NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page3,
                           "page-actions", &actions,
                           NULL);
  g_assert (actions == FALSE);
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
test_pm_get_set_page_custom_toolbox (PageManagerFixture *fixture,
                                     gconstpointer unused)
{
  GtkWidget *new_tb = gtk_label_new ("Another toolbox");
  GtkWidget *tb;

  tb = eos_page_manager_get_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                        fixture->page1);
  g_assert (tb == NULL);
  tb = eos_page_manager_get_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                        fixture->page2);
  g_assert (tb == fixture->toolbox2);

  eos_page_manager_set_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                   fixture->page1,
                                                   new_tb);
  eos_page_manager_set_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                   fixture->page2,
                                                   new_tb);

  tb = eos_page_manager_get_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                        fixture->page1);
  g_assert (tb == new_tb);
  tb = eos_page_manager_get_page_custom_toolbox_widget (EOS_PAGE_MANAGER (fixture->pm),
                                                        fixture->page2);
  g_assert (tb == new_tb);
}

static void
test_pm_child_prop_custom_toolbox (PageManagerFixture *fixture,
                                   gconstpointer unused)
{
  GtkWidget *new_tb = gtk_label_new ("Another toolbox");
  GtkWidget *tb;

  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "custom-toolbox-widget", &tb,
                           NULL);
  g_assert (tb == NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "custom-toolbox-widget", &tb,
                           NULL);
  g_assert (tb == fixture->toolbox2);

  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "custom-toolbox-widget", new_tb,
                           NULL);
  gtk_container_child_set (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "custom-toolbox-widget", new_tb,
                           NULL);

  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page1,
                           "custom-toolbox-widget", &tb,
                           NULL);
  g_assert (tb == new_tb);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), fixture->page2,
                           "custom-toolbox-widget", &tb,
                           NULL);
  g_assert (tb == new_tb);
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
test_pm_page_no_name (PageManagerFixture *fixture,
                      gconstpointer       unused)
{
  const gchar *name_get;
  gchar *name_prop;
  GtkWidget *new_page = gtk_label_new("new");
  gtk_container_add (GTK_CONTAINER (fixture->pm), new_page);
  name_get = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm), new_page);
  g_assert (name_get == NULL);
  gtk_container_child_get (GTK_CONTAINER (fixture->pm), new_page,
                           "name", &name_prop,
                           NULL);
  g_assert (name_prop == NULL);
}

static void
test_pm_set_page_no_name (PageManagerFixture *fixture,
                          gconstpointer       unused)
{
  const gchar *name;
  eos_page_manager_set_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                  fixture->page1,
                                  NULL);
  name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                         fixture->page1);
  g_assert (name == NULL);
  name = eos_page_manager_get_visible_page_name (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (name == NULL);
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
test_pm_remove_page_behavior (PageManagerFixture *fixture,
                              gconstpointer       unused)
{
  GtkWidget *visible_page;
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == fixture->page1);
  gtk_container_remove (GTK_CONTAINER (fixture->pm), fixture->page3);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == fixture->page1);
  gtk_container_remove (GTK_CONTAINER (fixture->pm), fixture->page2);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == fixture->page1);
  gtk_container_remove (GTK_CONTAINER (fixture->pm), fixture->page1);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == NULL);
}

static void
test_pm_remove_page_undefined_behavior (PageManagerFixture *fixture,
                                        gconstpointer       unused)
{
  GtkWidget *visible_page;
  GtkWidget *page1 = gtk_label_new ("page1");
  GtkWidget *page2 = gtk_label_new ("page2");
  gtk_container_add (GTK_CONTAINER (fixture->pm), page1);
  gtk_container_add (GTK_CONTAINER (fixture->pm), page2);
  eos_page_manager_set_visible_page (EOS_PAGE_MANAGER (fixture->pm),
                                     page1);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == page1);
  gtk_container_remove (GTK_CONTAINER (fixture->pm), page1);
  // Only one page left, so it should be the visible page.
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == page2);
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

static void
test_pm_duplicate_page_name (PageManagerFixture *fixture,
                             gconstpointer unused)
{
  eos_page_manager_set_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                  fixture->page1,
                                  DUPLICATE_PAGE_NAME);
  /* Should not complain */
  eos_page_manager_set_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                  fixture->page1,
                                  DUPLICATE_PAGE_NAME);

  g_test_expect_message (TEST_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,
                         EXPECTED_DUPLICATE_PAGE_NAME_ERRMSG);
  eos_page_manager_set_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                  fixture->page2,
                                  DUPLICATE_PAGE_NAME);
  g_test_assert_expected_messages ();

  const gchar *name = eos_page_manager_get_page_name (EOS_PAGE_MANAGER (fixture->pm),
                                                      fixture->page2);
  g_assert_cmpstr (name, !=, DUPLICATE_PAGE_NAME);
}

static void
test_pm_prop_transition_duration (PageManagerFixture *fixture,
                                  gconstpointer       unused)
{
  guint duration;
  g_object_get (fixture->pm, "transition-duration", &duration, NULL);
  g_assert (duration == DURATION_DEFAULT);
  g_object_set (fixture->pm, "transition-duration", DURATION_2, NULL);
  g_object_get (fixture->pm, "transition-duration", &duration, NULL);
  g_assert (duration == DURATION_2);
}

static void
test_pm_get_set_transition_duration (PageManagerFixture *fixture,
                                     gconstpointer       unused)
{
  g_assert (DURATION_DEFAULT == eos_page_manager_get_transition_duration (EOS_PAGE_MANAGER (fixture->pm)));
  eos_page_manager_set_transition_duration (EOS_PAGE_MANAGER (fixture->pm), DURATION_1);
  g_assert (DURATION_1 == eos_page_manager_get_transition_duration (EOS_PAGE_MANAGER (fixture->pm)));
  eos_page_manager_set_transition_duration (EOS_PAGE_MANAGER (fixture->pm), DURATION_2);
  g_assert (DURATION_2 == eos_page_manager_get_transition_duration (EOS_PAGE_MANAGER (fixture->pm)));
}

static void
test_pm_prop_transition_type (PageManagerFixture *fixture,
                                  gconstpointer       unused)
{
  EosPageManagerTransitionType type;
  g_object_get (fixture->pm, "transition-type", &type, NULL);
  g_assert (type == EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE);
  g_object_set (fixture->pm, "transition-type", EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE, NULL);
  g_object_get (fixture->pm, "transition-type", &type, NULL);
  g_assert (type == EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE);
}

static void
test_pm_get_set_transition_type (PageManagerFixture *fixture,
                                 gconstpointer       unused)
{
  g_assert (EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE == eos_page_manager_get_transition_type (EOS_PAGE_MANAGER (fixture->pm)));
  eos_page_manager_set_transition_type (EOS_PAGE_MANAGER (fixture->pm), EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE);
  g_assert (EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE == eos_page_manager_get_transition_type (EOS_PAGE_MANAGER (fixture->pm)));
}

static void
test_empty_pm_visible_page (PageManagerFixture *fixture,
                            gconstpointer       unused)
{
  GtkWidget *visible_page_get, *visible_page_prop;
  visible_page_get = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page_get == NULL);
  g_object_get (fixture->pm, "visible-page", &visible_page_prop, NULL);
  g_assert (visible_page_prop == NULL);
}

static void
test_empty_pm_visible_page_name (PageManagerFixture *fixture,
                                 gconstpointer       unused)
{
  const gchar *name_get;
  gchar *name_prop;
  name_get = eos_page_manager_get_visible_page_name (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (name_get == NULL);
  g_object_get (fixture->pm, "visible-page-name", &name_prop, NULL);
  g_assert (name_prop == NULL);
}

static void
test_empty_pm_add_page_behavior (PageManagerFixture *fixture,
                                 gconstpointer       unused)
{
  GtkWidget *visible_page;
  GtkWidget *page1 = gtk_label_new ("page1");
  GtkWidget *page2 = gtk_label_new ("page2");
  gtk_container_add (GTK_CONTAINER (fixture->pm), page1);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == page1);
  gtk_container_add (GTK_CONTAINER (fixture->pm), page2);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->pm));
  g_assert (visible_page == page1); /* Not page2! */
}

void
add_page_manager_tests (void)
{
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-visible-page",
                         test_pm_get_set_visible_page);
  ADD_PAGE_MANAGER_TEST ("/page-manager/prop-visible-page",
                         test_pm_prop_visible_page);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-visible-page-name",
                         test_pm_get_set_visible_page_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/prop-visible-page-name",
                         test_pm_prop_visible_page_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-page-name",
                         test_pm_get_set_page_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/child-prop-name",
                         test_pm_child_prop_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-page-actions",
                         test_pm_get_set_page_actions);
  ADD_PAGE_MANAGER_TEST ("/page-manager/child-prop-page-actions",
                         test_pm_child_prop_page_actions);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-page-custom-toolbox",
                         test_pm_get_set_page_custom_toolbox);
  ADD_PAGE_MANAGER_TEST ("/page-manager/child-prop-custom-toolbox",
                         test_pm_child_prop_custom_toolbox);
  ADD_PAGE_MANAGER_TEST ("/page-manager/page-no-name", test_pm_page_no_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/set-page-no-name",
                         test_pm_set_page_no_name);
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
  ADD_PAGE_MANAGER_TEST ("/page-manager/duplicate-page-name",
                         test_pm_duplicate_page_name);
  ADD_PAGE_MANAGER_TEST ("/page-manager/prop-transition-duration",
                         test_pm_prop_transition_duration);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-transition-duration",
                         test_pm_get_set_transition_duration);
  ADD_PAGE_MANAGER_TEST ("/page-manager/prop-transition-type",
                         test_pm_prop_transition_type);
  ADD_PAGE_MANAGER_TEST ("/page-manager/get-set-transition-type",
                         test_pm_get_set_transition_type);
  ADD_EMPTY_PAGE_MANAGER_TEST ("/page-manager/empty-visible-page",
                               test_empty_pm_visible_page);
  ADD_EMPTY_PAGE_MANAGER_TEST ("/page-manager/empty-visible-page-name",
                               test_empty_pm_visible_page_name);
  ADD_EMPTY_PAGE_MANAGER_TEST ("/page-manager/add-page-behavior",
                               test_empty_pm_add_page_behavior);

  /* Disabled until https://bugzilla.gnome.org/show_bug.cgi?id=699756 is fixed
  [endlessm/eos-sdk#67] */
  if (FALSE)
    {
      ADD_PAGE_MANAGER_TEST ("/page-manager/remove-page-behavior",
                           test_pm_remove_page_behavior);
      ADD_EMPTY_PAGE_MANAGER_TEST ("/page-manager/remove-page-undefined-behavior",
                                   test_pm_remove_page_undefined_behavior);
    }
}
