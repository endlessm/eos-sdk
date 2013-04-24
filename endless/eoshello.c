/* Copyright 2013 Endless Mobile, Inc. */

#include <config.h>
#include <string.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <gio/gio.h>

#include <endless/endless.h>

/**
 * SECTION:hello
 * @short_description: Sample skeleton function
 * @title: Hello
 *
 * This is a sample skeleton function that says hello either to the terminal or
 * a file.
 */

/**
 * eos_hello_sample_function:
 * @file: (allow-none): #GFile to write to, or %NULL
 * @error: (allow-none): Return location for a #GError, or %NULL to ignore.
 *
 * A sample API function to say hello with. Prints on the terminal if @file is
 * %NULL, or else appends it to @file.
 *
 * Returns: %TRUE on success, %FALSE on error.
 */
gboolean
eos_hello_sample_function(GFile   *file,
                          GError **error)
{
  char *hello_string = _("Hello, world!\n");
  GFileOutputStream *stream;
  ssize_t write_count;
  gboolean success;

  g_return_val_if_fail (G_IS_FILE (file) || file == NULL, FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  /* Print to terminal */
  if (file == NULL)
    {
      g_print ("%s", hello_string);
      return TRUE;
    }

  stream = g_file_append_to (file,
                             G_FILE_CREATE_NONE,
                             NULL, /* cancellable */
                             error);
  if(!stream)
    return FALSE;

  write_count = g_output_stream_write (G_OUTPUT_STREAM (stream),
                                       hello_string,
                                       strlen (hello_string),
                                       NULL, /* cancellable */
                                       error);
  success = g_output_stream_close (G_OUTPUT_STREAM (stream),
                                   NULL, /* cancellable */
                                   error);
  g_object_unref (stream);

  if (write_count == -1 || !success)
    return FALSE;

  return TRUE;
}
