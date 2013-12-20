/* Copyright 2013 Endless Mobile, Inc. */

#include <stdlib.h>
#include <glib.h>
#include <endless/endless.h>

#include "run-tests.h"

#define EXPECTED_HELLO_STRING "Hello, world!\n"

static void
test_hello_stdout (void)
{
  GError *error = NULL;

  /* Unix-only test */
  if (g_test_trap_fork(0 /* timeout */, G_TEST_TRAP_SILENCE_STDOUT))
    {
      gboolean success = eos_hello_sample_function (NULL, &error);
      g_assert (success);
      g_assert_no_error (error);
      exit (0);
    }

  g_test_trap_assert_passed ();
  g_test_trap_assert_stdout (EXPECTED_HELLO_STRING);
}

static void
test_hello_gfile (void)
{
  GError *error = NULL;
  GFileIOStream *stream;
  GFile *file = g_file_new_tmp ("sdktestXXXXXX", &stream, &error);
  gboolean success;
  char *file_contents;

  g_assert_no_error (error);
  g_io_stream_close (G_IO_STREAM (stream), NULL, &error);
  g_assert_no_error (error);

  success = eos_hello_sample_function (file, &error);
  g_assert (success);
  g_assert_no_error (error);

  g_file_load_contents (file, NULL, &file_contents, NULL, NULL, &error);
  g_assert_no_error (error);
  g_assert_cmpstr (file_contents, ==, EXPECTED_HELLO_STRING);

  g_free (file_contents);
  g_object_unref (file);
}

void
add_hello_tests (void)
{
  g_test_add_func ("/hello/stdout", test_hello_stdout);
  g_test_add_func ("/hello/gfile", test_hello_gfile);
}
