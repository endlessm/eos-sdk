/* Copyright 2013 Endless Mobile, Inc. */

#ifndef RUN_TESTS_H
#define RUN_TESTS_H

#define TEST_LOG_DOMAIN "EndlessSDK"

#define ADD_APP_WINDOW_TEST(path, test_func) \
  g_test_add ((path), AppWindowTestFixture, (test_func), \
              app_window_test_fixture_setup, \
              app_window_test_fixture_test, \
              app_window_test_fixture_teardown);

typedef struct
{
  EosApplication *app;
} AppWindowTestFixture;

gchar     *generate_unique_app_id              (void);

void       app_window_test_fixture_setup       (AppWindowTestFixture *fixture,
                                                gconstpointer callback);

void       app_window_test_fixture_test        (AppWindowTestFixture *fixture,
                                                gconstpointer unused);

void       app_window_test_fixture_teardown    (AppWindowTestFixture *fixture,
                                                gconstpointer unused);

GList     *container_get_all_children          (GtkContainer *container);

GtkWidget *container_find_descendant_with_type (GtkContainer *container,
                                                GType type);

void add_init_tests                          (void);
void add_hello_tests                         (void);
void add_application_tests                   (void);
void add_window_tests                        (void);
void add_page_manager_tests                  (void);
void add_action_menu_tests                   (void);
void add_action_button_tests                 (void);
void add_flexy_grid_test                     (void);
void add_custom_container_tests              (void);
void add_profile_tests                       (void);

#endif /* RUN_TESTS_H */
