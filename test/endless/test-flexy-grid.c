#include <gtk/gtk.h>
#include <endless/endless.h>

#include <endless/eosflexygrid-private.h>

#include "run-tests.h"

G_GNUC_BEGIN_IGNORE_DEPRECATIONS

typedef struct
{
  EosFlexyGrid *grid;
} FlexyGridFixture;

#define ADD_FLEXY_GRID_TEST(path, test_func) \
  g_test_add ((path), FlexyGridFixture, NULL, \
              flexy_grid_fixture_setup, \
              (test_func), \
              flexy_grid_fixture_teardown)


static void
flexy_grid_fixture_setup (FlexyGridFixture *fixture,
                          gconstpointer     unused G_GNUC_UNUSED)
{
  fixture->grid = (EosFlexyGrid *) eos_flexy_grid_new ();
}

static void
flexy_grid_fixture_teardown (FlexyGridFixture *fixture,
                             gconstpointer     unused G_GNUC_UNUSED)
{
  gtk_widget_destroy ((GtkWidget *) fixture->grid);
}

static void
flexy_grid_cell_size_access (FlexyGridFixture *fixture,
                             gconstpointer     unused G_GNUC_UNUSED)
{
  eos_flexy_grid_set_cell_size (fixture->grid, 6);
  g_assert_cmpint (eos_flexy_grid_get_cell_size (fixture->grid), ==, 6);

  eos_flexy_grid_set_cell_size (fixture->grid, -1);
  g_assert_cmpint (eos_flexy_grid_get_cell_size (fixture->grid), !=, -1);

  int cell_size = 0;
  g_object_get (fixture->grid, "cell-size", &cell_size, NULL);
  g_assert_cmpint (cell_size, !=, -1);

  g_object_set (fixture->grid, "cell-size", 250, NULL);
  g_assert_cmpint (eos_flexy_grid_get_cell_size (fixture->grid), ==, 250);
}

static void
flexy_grid_cell_spacing_access (FlexyGridFixture *fixture,
                                gconstpointer     unused G_GNUC_UNUSED)
{
  eos_flexy_grid_set_cell_spacing (fixture->grid, 6);
  g_assert_cmpint (eos_flexy_grid_get_cell_spacing (fixture->grid), ==, 6);

  eos_flexy_grid_set_cell_spacing (fixture->grid, -1);
  g_assert_cmpint (eos_flexy_grid_get_cell_spacing (fixture->grid), !=, -1);

  int cell_spacing = 0;
  g_object_get (fixture->grid, "cell-spacing", &cell_spacing, NULL);
  g_assert_cmpint (cell_spacing, !=, -1);

  g_object_set (fixture->grid, "cell-spacing", 12, NULL);
  g_assert_cmpint (eos_flexy_grid_get_cell_spacing (fixture->grid), ==, 12);
}

void
add_flexy_grid_test (void)
{
  ADD_FLEXY_GRID_TEST ("/flexy-grid/get-set-cell-size", flexy_grid_cell_size_access);
  ADD_FLEXY_GRID_TEST ("/flexy-grid/get-set-cell-spacing", flexy_grid_cell_spacing_access);
}

G_GNUC_END_IGNORE_DEPRECATIONS
