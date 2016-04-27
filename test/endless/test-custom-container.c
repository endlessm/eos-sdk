/* Copyright (C) 2014-2016 Endless Mobile, Inc. */

#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

typedef struct
{
  GtkContainer *container;
  GtkWidget *child1;
  GtkWidget *child2;
  GtkWidget *child3;
} CustomContainerFixture;

#define ADD_CUSTOM_CONTAINER_TEST(path, test_func) \
  g_test_add ((path), CustomContainerFixture, NULL, \
              custom_container_fixture_setup, \
              (test_func), \
              custom_container_fixture_teardown)


static void
custom_container_fixture_setup (CustomContainerFixture *fixture,
                                gconstpointer           unused G_GNUC_UNUSED)
{
  // We acquire the widget ref so they don't automatically get destroyed after
  // being removed from the container.
  fixture->child1 = g_object_ref_sink (gtk_label_new ("1"));
  fixture->child2 = g_object_ref_sink (gtk_label_new ("2"));
  fixture->child3 = g_object_ref_sink (gtk_label_new ("3"));
  fixture->container = GTK_CONTAINER (eos_custom_container_new ());
}

static void
custom_container_fixture_teardown (CustomContainerFixture *fixture,
                                   gconstpointer           unused G_GNUC_UNUSED)
{
  gtk_widget_destroy (fixture->child1);
  gtk_widget_destroy (fixture->child2);
  gtk_widget_destroy (fixture->child3);
  gtk_widget_destroy ((GtkWidget *) fixture->container);
  g_object_unref (fixture->child1);
  g_object_unref (fixture->child2);
  g_object_unref (fixture->child3);
}

static void
test_custom_container_add (CustomContainerFixture *fixture,
                           gconstpointer           unused G_GNUC_UNUSED)
{
  gtk_container_add (fixture->container, fixture->child1);
  gtk_container_add (fixture->container, fixture->child2);
  gtk_container_add (fixture->container, fixture->child3);

  g_assert (gtk_widget_get_parent (fixture->child1) == GTK_WIDGET (fixture->container));
  GList *children = gtk_container_get_children (fixture->container);
  g_assert (g_list_length (children) == 3);
  g_assert (g_list_find (children, fixture->child1) != NULL);
  g_assert (g_list_find (children, fixture->child2) != NULL);
  g_assert (g_list_find (children, fixture->child3) != NULL);
}

static void
test_custom_container_remove (CustomContainerFixture *fixture,
                              gconstpointer           unused G_GNUC_UNUSED)
{
  gtk_container_add (fixture->container, fixture->child1);
  gtk_container_add (fixture->container, fixture->child2);
  gtk_container_add (fixture->container, fixture->child3);
  gtk_container_remove (fixture->container, fixture->child2);

  g_assert (gtk_widget_get_parent (fixture->child2) != GTK_WIDGET (fixture->container));
  GList *children = gtk_container_get_children (fixture->container);
  g_assert (g_list_find (children, fixture->child2) == NULL);
}

void
add_custom_container_tests (void)
{
  ADD_CUSTOM_CONTAINER_TEST ("/custom-container/add", test_custom_container_add);
  ADD_CUSTOM_CONTAINER_TEST ("/custom-container/remove", test_custom_container_remove);
}
