#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define ADD_ACTION_MENU_TEST(path, test_func) \
    g_test_add ((path), ActionMenuFixture, NULL, \
                am_fixture_setup, (test_func), am_fixture_teardown)

typedef struct
{
  GtkWidget *action_menu;
  GtkAction *action1;
  GtkAction *action2;
  GtkAction *action3;
} ActionMenuFixture;

static void
am_fixture_setup (ActionMenuFixture *fixture,
                  gconstpointer       unused)
{
  fixture->action_menu = eos_action_menu_new ();
  fixture->action1 = gtk_action_new ("1", "1", "1", "1");
  fixture->action2 = gtk_action_new ("2", "2", "2", "2");
  fixture->action3 = gtk_action_new ("3", "3", "3", "3");
}

static void
am_fixture_teardown (ActionMenuFixture *fixture,
                     gconstpointer       unused)
{
  gtk_widget_destroy (fixture->action_menu);
}

/* TESTS */

static void
test_am_add_action (ActionMenuFixture *fixture,
                    gconstpointer       unused)
{

}

static void
test_am_get_action (ActionMenuFixture *fixture,
                    gconstpointer       unused)
{

}

static void
test_am_list_actions (ActionMenuFixture *fixture,
                      gconstpointer       unused)
{

}

static void
test_am_remove_action (ActionMenuFixture *fixture,
                       gconstpointer       unused)
{

}

static void
test_am_remove_action_by_name (ActionMenuFixture *fixture,
                               gconstpointer       unused)
{

}

void
add_action_menu_tests (void)
{
  ADD_ACTION_MENU_TEST ("/action-menu/add-action",
                        test_am_add_action);
  ADD_ACTION_MENU_TEST ("/action-menu/get-action",
                        test_am_get_action);
  ADD_ACTION_MENU_TEST ("/action-menu/list-actions",
                        test_am_list_actions);
  ADD_ACTION_MENU_TEST ("/action-menu/remove-action",
                        test_am_remove_action);
  ADD_ACTION_MENU_TEST ("/action-menu/remove-action-by-name",
                        test_am_remove_action_by_name);
}
