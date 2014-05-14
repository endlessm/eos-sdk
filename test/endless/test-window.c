/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>
#include "endless/eostopbar-private.h"

#include "run-tests.h"

#define EXPECTED_TOP_BAR_HEIGHT 32
#define EXPECTED_NULL_APPLICATION_ERRMSG \
  "*In order to create a window, you must have an application for it to " \
  "connect to.*"

static void
test_assign_application (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  g_assert(EOS_APPLICATION (app)
           == EOS_APPLICATION (gtk_window_get_application (GTK_WINDOW (win))));

  gtk_widget_destroy (win);
}

static void
test_application_not_null (GApplication *app)
{
  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDERR))
    {
      GtkWidget *win = eos_window_new (NULL);
      gtk_widget_destroy (win);
      exit (0);
    }

  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr (EXPECTED_NULL_APPLICATION_ERRMSG);

  g_application_quit (app); /* No window, so otherwise won't quit */
}

static void
test_has_top_bar (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  GtkWidget *top_bar = container_find_descendant_with_type (GTK_CONTAINER (win), EOS_TYPE_TOP_BAR);
  g_assert (top_bar != NULL);
  g_assert (EOS_IS_TOP_BAR (top_bar));

  gtk_widget_destroy (win);
}

static void
test_has_default_page_manager (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  EosPageManager *pm = eos_window_get_page_manager (EOS_WINDOW (win));
  g_assert (pm != NULL);

  g_object_get (win, "page-manager", &pm, NULL);
  g_assert (pm != NULL);

  gtk_widget_destroy (win);
}

static void
test_get_set_page_manager (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  EosPageManager *orig_pm = eos_window_get_page_manager (EOS_WINDOW (win));
  EosPageManager *new_pm = EOS_PAGE_MANAGER (eos_page_manager_new ());

  g_assert (orig_pm != new_pm);
  eos_window_set_page_manager(EOS_WINDOW (win), new_pm);
  EosPageManager *test_pm = eos_window_get_page_manager (EOS_WINDOW (win));
  g_assert (new_pm == test_pm);

  gtk_widget_destroy (win);
}

static void
test_get_set_font_scaling_active (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  gboolean is_scaling_default = eos_window_get_font_scaling_active (EOS_WINDOW (win));
  g_assert (!is_scaling_default);

  eos_window_set_font_scaling_active (EOS_WINDOW (win), TRUE);
  gboolean is_scaling = eos_window_get_font_scaling_active (EOS_WINDOW (win));
  g_assert (is_scaling);

  gtk_widget_destroy (win);
}

static void
test_get_set_font_scaling_default_size (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  gint new_font_size = 10;

  eos_window_set_font_scaling_default_size (EOS_WINDOW (win), new_font_size);
  gint returned_font_size = eos_window_get_font_scaling_default_size (EOS_WINDOW (win));

  g_assert (new_font_size == returned_font_size);

  gtk_widget_destroy (win);
}

static void
test_get_set_font_scaling_default_window_size (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  gint new_window_size = 720;

  eos_window_set_font_scaling_default_window_size (EOS_WINDOW (win), new_window_size);
  gint returned_window_size = eos_window_get_font_scaling_default_window_size (EOS_WINDOW (win));

  g_assert (new_window_size == returned_window_size);

  gtk_widget_destroy (win);
}

static void
test_get_set_font_scaling_min_font_size (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  gint new_min_font_size = 10;

  eos_window_set_font_scaling_min_font_size (EOS_WINDOW (win), new_min_font_size);
  gint returned_min_font_size = eos_window_get_font_scaling_min_font_size (EOS_WINDOW (win));

  g_assert (new_min_font_size == returned_min_font_size);

  gtk_widget_destroy (win);
}

static void
test_prop_page_manager (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  EosPageManager *orig_pm;
  g_object_get(win, "page-manager", &orig_pm, NULL);
  EosPageManager *new_pm = EOS_PAGE_MANAGER (eos_page_manager_new ());

  g_assert (orig_pm != new_pm);
  g_object_set(win, "page-manager", new_pm, NULL);
  EosPageManager *test_pm;
  g_object_get(win, "page-manager", &test_pm, NULL);
  g_assert (new_pm == test_pm);

  gtk_widget_destroy (win);
}

static void
test_internal_widget_visibility (GApplication *app) {
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  EosPageManager *pm = eos_window_get_page_manager (EOS_WINDOW (win));
  GtkWidget *page0 = gtk_label_new ("test");

  gtk_container_add (GTK_CONTAINER (pm), page0);
  gtk_widget_show (page0);
  gtk_widget_show (win);

  // We have a lot of internal widgets, if we forgotten to call show on one of
  // them the label won't be visible, even though we just called show on the
  // two widgets we created in this test.
  g_assert (gtk_widget_is_visible (page0));

  gtk_widget_destroy (win);
}


void
add_window_tests (void)
{
  ADD_APP_WINDOW_TEST ("/window/assign-application", test_assign_application);
  ADD_APP_WINDOW_TEST ("/window/application-not-null",
                       test_application_not_null);
  ADD_APP_WINDOW_TEST ("/window/has-top-bar", test_has_top_bar);
  ADD_APP_WINDOW_TEST ("/window/has-default-page-manager",
                       test_has_default_page_manager);
  ADD_APP_WINDOW_TEST ("/window/get-set-page-manager",
                       test_get_set_page_manager);
  ADD_APP_WINDOW_TEST ("/window/get-set-font-scaling-active",
                       test_get_set_font_scaling_active);
  ADD_APP_WINDOW_TEST ("/window/get-set-font-scaling-default-size",
                       test_get_set_font_scaling_default_size);
  ADD_APP_WINDOW_TEST ("/window/get-set-font-scaling-default-window-size",
                       test_get_set_font_scaling_default_window_size);
  ADD_APP_WINDOW_TEST ("/window/get-set-font-scaling-min-font-size",
                       test_get_set_font_scaling_min_font_size);
  ADD_APP_WINDOW_TEST ("/window/prop-page-manager", test_prop_page_manager);
  ADD_APP_WINDOW_TEST ("/window/internal-widget-visibility",
                       test_internal_widget_visibility);
}
