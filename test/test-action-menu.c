#include <gtk/gtk.h>
#include <endless/endless.h>

#include <endless/eosactionmenu-private.h>

#include "run-tests.h"

#include "endless/eosactionmenu.c"

#define ADD_ACTION_MENU_TEST(path, test_func) \
    g_test_add ((path), ActionMenuFixture, NULL, \
                am_fixture_setup, (test_func), am_fixture_teardown)

typedef struct
{
  EosActionMenu *action_menu;
  GtkAction *action1;
  GtkAction *action2;
  GtkAction *action3;
} ActionMenuFixture;

static void
am_fixture_setup (ActionMenuFixture *fixture,
                  gconstpointer      unused)
{
  fixture->action_menu = EOS_ACTION_MENU (eos_action_menu_new ());
  fixture->action1 = gtk_action_new ("1", "1", "1", "1");
  fixture->action2 = gtk_action_new ("2", "2", "2", "2");
  fixture->action3 = gtk_action_new ("3", "3", "3", "3");

  g_object_ref (fixture->action1);
  g_object_ref (fixture->action2);
  g_object_ref (fixture->action3);
}

static void
am_fixture_teardown (ActionMenuFixture *fixture,
                     gconstpointer      unused)
{
  gtk_widget_destroy (GTK_WIDGET (fixture->action_menu));
  g_object_unref (fixture->action1);
  g_object_unref (fixture->action2);
  g_object_unref (fixture->action3);
}

/* TESTS */

static void
test_am_add_action (ActionMenuFixture *fixture,
                    gconstpointer      unused)
{
  gint size;
  gchar *label, *icon_id;

  gtk_action_set_is_important (fixture->action1, TRUE);
  gtk_action_set_icon_name (fixture->action1, "object-select-symbolic");

  eos_action_menu_add_action (fixture->action_menu, fixture->action1);

  GtkWidget *button = gtk_grid_get_child_at (GTK_GRID (fixture->action_menu->priv->grid), 0, 0);

  g_assert (EOS_IS_ACTION_BUTTON (button));

  g_object_get (button,
                "size", &size,
                "label", &label,
                "icon-id", &icon_id,
                NULL);

  g_assert ( size == EOS_ACTION_BUTTON_SIZE_PRIMARY);
  g_assert ( g_strcmp0 (label, gtk_action_get_label (fixture->action1)) == 0);
  g_assert ( g_strcmp0 (icon_id, gtk_action_get_icon_name (fixture->action1)) == 0);

  g_free (label);
  g_free (icon_id);
}

static void
test_am_get_action (ActionMenuFixture *fixture,
                    gconstpointer      unused)
{
  eos_action_menu_add_action (fixture->action_menu, fixture->action1);

  GtkAction *retrieved = eos_action_menu_get_action (fixture->action_menu, "1");

  g_assert (retrieved == fixture->action1);
}

static void
test_am_list_actions (ActionMenuFixture *fixture,
                      gconstpointer      unused)
{
  GList *list = eos_action_menu_list_actions (fixture->action_menu);

  g_assert (list == NULL);

  eos_action_menu_add_action (fixture->action_menu, fixture->action1);
  eos_action_menu_add_action (fixture->action_menu, fixture->action2);

  list = eos_action_menu_list_actions (fixture->action_menu);

  g_assert (g_list_find (list, fixture->action1) != NULL);
  g_assert (g_list_find (list, fixture->action2) != NULL);

  g_assert (g_list_find (list, fixture->action3) == NULL);
}

static gboolean
menu_contains_button_with_label (GtkContainer *menu, const gchar* button_label)
{
  GList* children = gtk_container_get_children (menu);
  gboolean found = FALSE;

  for (GList *i = children; i != NULL ; i = i->next)
    {
      if (EOS_IS_ACTION_BUTTON (i->data))
        {
          if (g_strcmp0 (eos_action_button_get_label (EOS_ACTION_BUTTON (i->data)),
                         button_label) == 0)
            {
              found = TRUE;
              break;
            }
        }
    }

  g_list_free (children);

  return found;
}

static void
test_am_remove_action (ActionMenuFixture *fixture,
                       gconstpointer      unused)
{
  GList *list;

  eos_action_menu_add_action (fixture->action_menu, fixture->action1);
  eos_action_menu_add_action (fixture->action_menu, fixture->action2);
  eos_action_menu_add_action (fixture->action_menu, fixture->action3);

  eos_action_menu_remove_action (fixture->action_menu, fixture->action2);

  list = eos_action_menu_list_actions (fixture->action_menu);

  g_assert (g_list_find (list, fixture->action1) != NULL);
  g_assert (g_list_find (list, fixture->action2) == NULL);
  g_assert (g_list_find (list, fixture->action3) != NULL);

  // the buttons have been removed as well
  g_assert (menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                             gtk_action_get_label (fixture->action1)));
  g_assert (!menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                              gtk_action_get_label (fixture->action2)));
  g_assert (menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                             gtk_action_get_label (fixture->action3)));

  eos_action_menu_remove_action (fixture->action_menu, fixture->action1);
  eos_action_menu_remove_action (fixture->action_menu, fixture->action3);

  list = eos_action_menu_list_actions (fixture->action_menu);

  g_assert (g_list_find (list, fixture->action1) == NULL);
  g_assert (g_list_find (list, fixture->action2) == NULL);
  g_assert (g_list_find (list, fixture->action3) == NULL);

  // the container is empty
  g_assert (gtk_container_get_children (GTK_CONTAINER (fixture->action_menu->priv->grid)) == NULL);
}

static void
test_am_remove_action_by_name (ActionMenuFixture *fixture,
                               gconstpointer      unused)
{
  eos_action_menu_add_action (fixture->action_menu, fixture->action1);
  eos_action_menu_add_action (fixture->action_menu, fixture->action2);
  eos_action_menu_add_action (fixture->action_menu, fixture->action3);

  eos_action_menu_remove_action_by_name (fixture->action_menu, "2");

  GList *list = eos_action_menu_list_actions (fixture->action_menu);

  g_assert (g_list_find (list, fixture->action1) != NULL);
  g_assert (g_list_find (list, fixture->action2) == NULL);
  g_assert (g_list_find (list, fixture->action3) != NULL);

  g_assert (menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                             gtk_action_get_label (fixture->action1)));
  g_assert (!menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                              gtk_action_get_label (fixture->action2)));
  g_assert (menu_contains_button_with_label (GTK_CONTAINER (fixture->action_menu->priv->grid),
                                             gtk_action_get_label (fixture->action3)));
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
