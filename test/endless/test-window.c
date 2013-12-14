/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>
#include "endless/eostopbar-private.h"
#include "endless/eosmainarea-private.h"

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
test_has_main_area (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  GtkWidget *main_area = container_find_descendant_with_type (GTK_CONTAINER (win), EOS_TYPE_MAIN_AREA);
  g_assert (main_area != NULL);
  g_assert (EOS_IS_MAIN_AREA (main_area));

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
test_main_area_widgets_visibility (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));
  EosPageManager *pm = eos_window_get_page_manager (EOS_WINDOW (win));
  GtkWidget *main_area = container_find_descendant_with_type (GTK_CONTAINER (win), EOS_TYPE_MAIN_AREA);

  GtkWidget *page0 = gtk_label_new ("no-no");
  GtkWidget *page1 = gtk_label_new ("yes-no");
  GtkWidget *page2 = gtk_label_new ("no-yes");
  GtkWidget *page3 = gtk_label_new ("yes-yes");

  GtkWidget *toolbox1 = gtk_label_new ("toolbox1");
  GtkWidget *toolbox3 = gtk_label_new ("toolbox3");

  gtk_container_add (GTK_CONTAINER (pm), page0);
  gtk_container_add_with_properties (GTK_CONTAINER (pm), page1,
                                     "custom-toolbox-widget", toolbox1,
                                     NULL);
  gtk_container_add_with_properties (GTK_CONTAINER (pm), page2,
                                     "page-actions", TRUE,
                                     NULL);
  gtk_container_add_with_properties (GTK_CONTAINER (pm), page3,
                                     "custom-toolbox-widget", toolbox3,
                                     "page-actions", TRUE,
                                     NULL);

  GtkWidget *tb;
  gboolean actions;
  EosMainArea *ma = EOS_MAIN_AREA (main_area);

  eos_page_manager_set_visible_page (pm, page0);
  tb = eos_main_area_get_toolbox (ma);
  actions = eos_main_area_get_actions (ma);
  g_assert (tb == NULL);
  g_assert (actions == FALSE);

  eos_page_manager_set_visible_page (pm, page1);
  tb = eos_main_area_get_toolbox (ma);
  actions = eos_main_area_get_actions (ma);
  g_assert (tb == toolbox1);
  g_assert (actions == FALSE);

  eos_page_manager_set_visible_page (pm, page2);
  tb = eos_main_area_get_toolbox (ma);
  actions = eos_main_area_get_actions (ma);
  g_assert (tb == NULL);
  g_assert (actions == TRUE);

  eos_page_manager_set_visible_page (pm, page3);
  tb = eos_main_area_get_toolbox (ma);
  actions = eos_main_area_get_actions (ma);
  g_assert (tb == toolbox3);
  g_assert (actions == TRUE);

  gtk_widget_destroy (win);
}

void
add_window_tests (void)
{
  ADD_APP_WINDOW_TEST ("/window/assign-application", test_assign_application);
  ADD_APP_WINDOW_TEST ("/window/application-not-null",
                       test_application_not_null);
  ADD_APP_WINDOW_TEST ("/window/has-top-bar", test_has_top_bar);
  ADD_APP_WINDOW_TEST ("/window/has-main-area", test_has_main_area);
  ADD_APP_WINDOW_TEST ("/window/has-default-page-manager",
                       test_has_default_page_manager);
  ADD_APP_WINDOW_TEST ("/window/get-set-page-manager",
                       test_get_set_page_manager);
  ADD_APP_WINDOW_TEST ("/window/prop-page-manager", test_prop_page_manager);
  ADD_APP_WINDOW_TEST ("/window/main-area-widgets-visibility",
                       test_main_area_widgets_visibility);
}
