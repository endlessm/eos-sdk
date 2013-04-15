/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <gtk/gtk.h>
#include <endless/endless.h>

#include "run-tests.h"

static void
test_assign_application (GApplication *app)
{
  GtkWidget *win = eos_window_new (EOS_APPLICATION (app));

  g_assert(EOS_APPLICATION (app)
           == EOS_APPLICATION (gtk_window_get_application (GTK_WINDOW (win))));

  gtk_widget_destroy (win);
}

void
add_window_tests (void)
{
  ADD_APP_WINDOW_TEST ("/window/assign-application", test_assign_application);
}
