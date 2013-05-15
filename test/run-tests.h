/* Copyright 2013 Endless Mobile, Inc. */

#ifndef RUN_TESTS_H
#define RUN_TESTS_H

#define TEST_LOG_DOMAIN "EndlessSDK"
#define TEST_APPLICATION_ID "com.endlessm.example.test"

#define ADD_APP_WINDOW_TEST(path, test_func) \
  g_test_add ((path), AppWindowTestFixture, (test_func), \
              app_window_test_fixture_setup, \
              app_window_test_fixture_test, \
              app_window_test_fixture_teardown);

typedef struct
{
  EosApplication *app;
} AppWindowTestFixture;

void app_window_test_fixture_setup    (AppWindowTestFixture *fixture,
                                       gconstpointer callback);
void app_window_test_fixture_test     (AppWindowTestFixture *fixture,
                                       gconstpointer unused);
void app_window_test_fixture_teardown (AppWindowTestFixture *fixture,
                                       gconstpointer unused);

void add_init_tests                   (void);
void add_hello_tests                  (void);
void add_application_tests            (void);
void add_window_tests                 (void);
void add_page_manager_tests           (void);
void add_splash_page_manager_tests           (void);

#endif /* RUN_TESTS_H */
