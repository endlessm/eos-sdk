#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

#define ADD_SPLASH_PAGE_MANAGER_TEST(path, test_func) \
  g_test_add ((path), SplashPageManagerFixture, NULL, \
              spm_fixture_setup, (test_func), spm_fixture_teardown)
#define ADD_EMPTY_SPLASH_PAGE_MANAGER_TEST(path, test_func) \
  g_test_add ((path), SplashPageManagerFixture, NULL, \
              empty_spm_fixture_setup, (test_func), spm_fixture_teardown);

typedef struct
{
  GtkWidget *spm;
  GtkWidget *first_splash_page;
  GtkWidget *second_splash_page;
  GtkWidget *first_main_page;
  GtkWidget *second_main_page;
} SplashPageManagerFixture;

static void
empty_spm_fixture_setup (SplashPageManagerFixture *fixture,
                         gconstpointer             unused)
{
  fixture->first_splash_page = gtk_label_new ("splash");
  fixture->second_splash_page = gtk_label_new ("ham sandwich");
  fixture->first_main_page = gtk_label_new ("main");
  fixture->second_main_page = gtk_label_new ("pikachu");
  fixture->spm = eos_splash_page_manager_new ();
}

static void
spm_fixture_setup (SplashPageManagerFixture *fixture,
                   gconstpointer             unused)
{
  empty_spm_fixture_setup (fixture, unused);
  eos_splash_page_manager_set_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                           fixture->first_splash_page);
  eos_splash_page_manager_set_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                         fixture->first_main_page);
}

static void
spm_fixture_teardown (SplashPageManagerFixture *fixture,
                      gconstpointer             unused)
{
  gtk_widget_destroy (fixture->spm);
}

static void
test_spm_get_set_splash_page (SplashPageManagerFixture *fixture,
                              gconstpointer             unused)
{
  GtkWidget *splash_page;
  splash_page = eos_splash_page_manager_get_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  g_assert (splash_page != fixture->second_splash_page);
  eos_splash_page_manager_set_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                           fixture->second_splash_page);
  splash_page = eos_splash_page_manager_get_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  g_assert (splash_page == fixture->second_splash_page);
}

static void
test_spm_prop_splash_page (SplashPageManagerFixture *fixture,
                           gconstpointer             unused)
{
  GtkWidget *splash_page;
  g_object_get (fixture->spm, "splash-page", &splash_page, NULL);
  g_assert (splash_page != fixture->second_splash_page);
  g_object_set (fixture->spm, "splash-page", fixture->second_splash_page, NULL);
  g_object_get (fixture->spm, "splash-page", &splash_page, NULL);
  g_assert (splash_page == fixture->second_splash_page);
}

static void
test_spm_get_set_main_page (SplashPageManagerFixture *fixture,
                            gconstpointer             unused)
{
  GtkWidget *main_page;
  main_page = eos_splash_page_manager_get_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  g_assert (main_page != fixture->second_main_page);
  eos_splash_page_manager_set_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                         fixture->second_main_page);
  main_page = eos_splash_page_manager_get_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  g_assert (main_page == fixture->second_main_page);
}

static void
test_spm_prop_main_page (SplashPageManagerFixture *fixture,
                         gconstpointer             unused)
{
  GtkWidget *main_page;
  g_object_get (fixture->spm, "main-page", &main_page, NULL);
  g_assert (main_page != fixture->second_main_page);
  g_object_set (fixture->spm, "main-page", fixture->second_main_page, NULL);
  g_object_get (fixture->spm, "main-page", &main_page, NULL);
  g_assert (main_page == fixture->second_main_page);
}

static void
test_spm_show_main_page (SplashPageManagerFixture *fixture,
                         gconstpointer             unused)
{
  GtkWidget *visible_page;
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page != fixture->first_main_page);
  eos_splash_page_manager_show_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page == fixture->first_main_page);
}


static void
test_spm_show_splash_page (SplashPageManagerFixture *fixture,
                           gconstpointer             unused)
{
  GtkWidget *visible_page;
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page == fixture->first_splash_page);
  eos_splash_page_manager_show_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page != fixture->first_splash_page);
  eos_splash_page_manager_show_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page == fixture->first_splash_page);
}

static void
test_spm_default_visible_splash (SplashPageManagerFixture *fixture,
                                 gconstpointer             unused)
{
  // Even though main page is added first splash page should be visible after it is added.
  GtkWidget *visible_page;
  eos_splash_page_manager_set_main_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                         fixture->first_main_page);
  eos_splash_page_manager_set_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm),
                                           fixture->first_splash_page);
  visible_page = eos_page_manager_get_visible_page (EOS_PAGE_MANAGER (fixture->spm));
  g_assert (visible_page == fixture->first_splash_page);
}

static void
test_spm_add_to_splash (SplashPageManagerFixture *fixture,
                        gconstpointer             unused)
{
  // Right now container add sets the splash page by default. This tests that
  // functionality.
  GtkWidget *splash_page;
  gtk_container_add (GTK_CONTAINER (fixture->spm),
                     fixture->first_splash_page);
  splash_page = eos_splash_page_manager_get_splash_page (EOS_SPLASH_PAGE_MANAGER (fixture->spm));
  g_assert (splash_page == fixture->first_splash_page);
}

void
add_splash_page_manager_tests (void)
{
  ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/show-main-page",
                                test_spm_show_main_page);
  ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/show-splash-page",
                                test_spm_show_splash_page);
  ADD_EMPTY_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/default-visible-splash",
                                      test_spm_default_visible_splash);
  ADD_EMPTY_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/add-to-splash",
                                      test_spm_add_to_splash);

  /* Disabled until https://bugzilla.gnome.org/show_bug.cgi?id=699756 is fixed
  [endlessm/eos-sdk#67] */
  if (FALSE)
    {
      ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/get-set-splash-page",
                                    test_spm_get_set_splash_page);
      ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/prop-splash-page",
                                    test_spm_prop_splash_page);
      ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/get-set-main-page",
                                    test_spm_get_set_main_page);
      ADD_SPLASH_PAGE_MANAGER_TEST ("/splash-page-manager/prop-main-page",
                                    test_spm_prop_main_page);
    }
}
