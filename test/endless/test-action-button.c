#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define ADD_ACTION_BUTTON_TEST(path, test_func) \
  g_test_add ((path), ActionButtonFixture, NULL, \
              ab_fixture_setup, (test_func), ab_fixture_teardown)

#define EXPECTED_DEFAULT_SIZE EOS_ACTION_BUTTON_SIZE_SECONDARY
#define EXPECTED_DEFAULT_LABEL NULL
#define EXPECTED_DEFAULT_ICON_NAME NULL

#define INITIAL_SIZE EOS_ACTION_BUTTON_SIZE_PRIMARY
#define INITIAL_LABEL "add"
#define INITIAL_ICON_NAME "list-add-symbolic"

/* When testing setters and getters, set values to these; should be different
from INITIAL_x */
#define EXPECTED_SIZE EOS_ACTION_BUTTON_SIZE_SECONDARY
#define EXPECTED_LABEL "remove"
#define EXPECTED_ICON_NAME "list-remove-symbolic"

typedef struct
{
  GtkWidget *window;
  EosActionButton *button;
} ActionButtonFixture;

static void
ab_fixture_setup (ActionButtonFixture *fixture,
                  gconstpointer        unused)
{
  fixture->button = EOS_ACTION_BUTTON (eos_action_button_new (INITIAL_SIZE,
                                                              INITIAL_LABEL,
                                                              INITIAL_ICON_NAME));

  /* Place it in an offscreen window so that we can test the size */
  fixture->window = gtk_offscreen_window_new ();
  gtk_container_add (GTK_CONTAINER (fixture->window),
                     GTK_WIDGET (fixture->button));

  gtk_widget_show_all (GTK_WIDGET (fixture->window));
}

static void
ab_fixture_teardown (ActionButtonFixture *fixture,
                     gconstpointer        unused)
{
  gtk_widget_destroy (GTK_WIDGET (fixture->window));
}

/* TESTS */

static void
test_ab_default_properties (void)
{
  /* Create it using g_object_new() so that the properties are not explicitly
  set */
  EosActionButton *button = EOS_ACTION_BUTTON (g_object_new (EOS_TYPE_ACTION_BUTTON,
                                                             NULL));
  EosActionButtonSize size = eos_action_button_get_size (button);
  const gchar *label = eos_action_button_get_label (button);
  const gchar *icon_name = eos_action_button_get_icon_id (button);

  g_assert_cmpint (size, ==, EXPECTED_DEFAULT_SIZE);
  g_assert_cmpstr (label, ==, EXPECTED_DEFAULT_LABEL);
  g_assert_cmpstr (icon_name, ==, EXPECTED_DEFAULT_ICON_NAME);
}

static void
test_ab_get_set_size (ActionButtonFixture *fixture,
                      gconstpointer        unused)
{
  EosActionButtonSize size;

  size = eos_action_button_get_size (fixture->button);
  g_assert_cmpint (size, ==, INITIAL_SIZE);

  eos_action_button_set_size (fixture->button, EXPECTED_SIZE);
  size = eos_action_button_get_size (fixture->button);
  g_assert_cmpint (size, ==, EXPECTED_SIZE);
}

static void
test_ab_prop_size (ActionButtonFixture *fixture,
                   gconstpointer        unused)
{
  EosActionButtonSize size;

  g_object_get (fixture->button, "size", &size, NULL);
  g_assert_cmpint (size, ==, INITIAL_SIZE);

  g_object_set (fixture->button, "size", EXPECTED_SIZE, NULL);
  g_object_get (fixture->button, "size", &size, NULL);
  g_assert_cmpint (size, ==, EXPECTED_SIZE);
}

static void
test_ab_get_set_label (ActionButtonFixture *fixture,
                       gconstpointer        unused)
{
  const gchar *label;

  label = eos_action_button_get_label (fixture->button);
  g_assert_cmpstr (label, ==, INITIAL_LABEL);

  eos_action_button_set_label (fixture->button, EXPECTED_LABEL);
  label = eos_action_button_get_label (fixture->button);
  g_assert_cmpstr (label, ==, EXPECTED_LABEL);
}

static void
test_ab_prop_label (ActionButtonFixture *fixture,
                    gconstpointer        unused)
{
  gchar *label;

  g_object_get (fixture->button, "label", &label, NULL);
  g_assert_cmpstr (label, ==, INITIAL_LABEL);
  g_free (label);

  g_object_set (fixture->button, "label", EXPECTED_LABEL, NULL);
  g_object_get (fixture->button, "label", &label, NULL);
  g_assert_cmpstr (label, ==, EXPECTED_LABEL);
  g_free (label);
}

static void
test_ab_get_set_icon_name (ActionButtonFixture *fixture,
                           gconstpointer        unused)
{
  const gchar *icon_name;

  icon_name = eos_action_button_get_icon_id (fixture->button);
  g_assert_cmpstr (icon_name, ==, INITIAL_ICON_NAME);

  eos_action_button_set_icon_id (fixture->button, EXPECTED_ICON_NAME);
  icon_name = eos_action_button_get_icon_id (fixture->button);
  g_assert_cmpstr (icon_name, ==, EXPECTED_ICON_NAME);
}

static void
test_ab_prop_icon_name (ActionButtonFixture *fixture,
                        gconstpointer        unused)
{
  gchar *icon_name;

  g_object_get (fixture->button, "icon-id", &icon_name, NULL);
  g_assert_cmpstr (icon_name, ==, INITIAL_ICON_NAME);
  g_free (icon_name);

  g_object_set (fixture->button, "icon-id", EXPECTED_ICON_NAME, NULL);
  g_object_get (fixture->button, "icon-id", &icon_name, NULL);
  g_assert_cmpstr (icon_name, ==, EXPECTED_ICON_NAME);
  g_free (icon_name);
}

static void
test_ab_get_set_label_position (ActionButtonFixture *fixture,
                                gconstpointer        unused)
{
  GtkPositionType label_pos;

  label_pos = eos_action_button_get_label_position (fixture->button);
  g_assert (label_pos == GTK_POS_BOTTOM);

  eos_action_button_set_label_position (fixture->button, GTK_POS_TOP);
  label_pos = eos_action_button_get_label_position (fixture->button);
  g_assert (label_pos == GTK_POS_TOP);
}

static void
test_ab_prop_label_position (ActionButtonFixture *fixture,
                             gconstpointer        unused)
{
  GtkPositionType label_pos;

  g_object_get (fixture->button, "label-position", &label_pos, NULL);
  g_assert (label_pos == GTK_POS_BOTTOM);

  g_object_set (fixture->button, "label-position", GTK_POS_TOP, NULL);
  g_object_get (fixture->button, "label-position", &label_pos, NULL);
  g_assert (label_pos == GTK_POS_TOP);
}

static void
test_ab_label_agrees (ActionButtonFixture *fixture,
                      gconstpointer        unused)
{
  GtkWidget *inner_label;
  const gchar *label_text;

  inner_label = container_find_descendant_with_type (GTK_CONTAINER (fixture->button),
                                                     GTK_TYPE_LABEL);
  g_assert (inner_label);

  label_text = gtk_label_get_text (GTK_LABEL (inner_label));
  g_assert_cmpstr (label_text, ==, INITIAL_LABEL);

  eos_action_button_set_label (fixture->button, EXPECTED_LABEL);
  label_text = gtk_label_get_text (GTK_LABEL (inner_label));
  g_assert_cmpstr (label_text, ==, EXPECTED_LABEL);
}

void
add_action_button_tests (void)
{
  g_test_add_func ("/action-button/default-properties",
                   test_ab_default_properties);
  ADD_ACTION_BUTTON_TEST ("/action-button/get-set-size", test_ab_get_set_size);
  ADD_ACTION_BUTTON_TEST ("/action-button/prop-size", test_ab_prop_size);
  ADD_ACTION_BUTTON_TEST ("/action-button/get-set-label",
                          test_ab_get_set_label);
  ADD_ACTION_BUTTON_TEST ("/action-button/prop-label", test_ab_prop_label);
  ADD_ACTION_BUTTON_TEST ("/action-button/get-set-icon-name",
                          test_ab_get_set_icon_name);
  ADD_ACTION_BUTTON_TEST ("/action-button/prop-icon-name",
                          test_ab_prop_icon_name);
  ADD_ACTION_BUTTON_TEST ("/action-button/get-set-label-position",
                          test_ab_get_set_label_position);
  ADD_ACTION_BUTTON_TEST ("/action-button/prop-label-position",
                          test_ab_prop_label_position);
  ADD_ACTION_BUTTON_TEST ("/action-button/label-agrees", test_ab_label_agrees);
}
