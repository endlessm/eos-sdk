#include "config.h"

#include "eos-profile-cmds.h"
#include "eos-profile-utils.h"

#include "endless/eosprofile-private.h"
#include "endless/gvdb/gvdb-reader.h"

#include <json-glib/json-glib.h>
#include <math.h>
#include <float.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

static const double
scale_val (double val)
{
  if (val >= G_USEC_PER_SEC)
    return val / G_USEC_PER_SEC;

  if (val >= 1000)
    return val / 1000.0;

  return val;
}

static const char *
unit_for (double val)
{
  enum {
    SECONDS,
    MILLISECONDS,
    MICROSECONDS
  };

  const char *units[] = {
    [SECONDS] = "s",
    [MILLISECONDS] = "ms",
    [MICROSECONDS] = "µs",
  };

  if (val >= G_USEC_PER_SEC)
    return units[SECONDS];

  if (val >= 1000)
    return units[MILLISECONDS];

  return units[MICROSECONDS];
}

static char **opt_files;
static char *opt_format;
static char *opt_output;

static GOptionEntry opts[] = {
  {
    .long_name = "format",
    .short_name = 'f',
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_STRING,
    .arg_data = &opt_format,
    .description = "The output format (valid values: plain, json)",
    .arg_description = "FORMAT",
  },
  {
    .long_name = "output",
    .short_name = 'o',
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_FILENAME,
    .arg_data = &opt_output,
    .description = "The output file, or '-' for the standard output",
    .arg_description = "FILE",
  },
  {
    .long_name = G_OPTION_REMAINING,
    .short_name = 0,
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_FILENAME_ARRAY,
    .arg_data = &opt_files,
    .description = "The files to compare",
    .arg_description = "FILES",
  },

  { NULL, },
};

static int output_fd = -1;
static char *output_tmpfile = NULL;

gboolean
eos_profile_cmd_diff_parse_args (int    argc,
                                 char **argv)
{
  g_autoptr(GError) error = NULL;

  g_autoptr(GOptionContext) context = g_option_context_new (NULL);

  g_option_context_set_help_enabled (context, TRUE);
  g_option_context_add_main_entries (context, opts, GETTEXT_PACKAGE);

  if (!g_option_context_parse (context, &argc, &argv, &error))
    {
      eos_profile_util_print_error ("Invalid argument: %s", error->message);
      return FALSE;
    }

  if (opt_format == NULL)
    opt_format = "plain";

  if (g_strcmp0 (opt_format, "plain") != 0 &&
      g_strcmp0 (opt_format, "json") != 0)
    {
      eos_profile_util_print_error ("Invalid output format");
      return FALSE;
    }

  if (opt_output == NULL)
    opt_output = "-";

  if (opt_output != NULL)
    {
      if (opt_output[0] == '-' && opt_output[1] == '\0')
        {
          output_fd = STDOUT_FILENO;
        }
      else
        {
          g_autoptr(GError) error = NULL;

          output_fd = g_file_open_tmp ("eos-profile-diff-XXXXXX", &output_tmpfile, &error);
          if (error != NULL)
            {
              eos_profile_util_print_error ("Unable to open output file: %s", error->message);
              return FALSE;
            }
        }
    }

  if (opt_files == NULL || g_strv_length (opt_files) < 2)
    {
      eos_profile_util_print_error ("Not enough files to compare");
      return FALSE;
    }

  return TRUE;
}

typedef struct {
  char *filename;
  double avg;
} ProbeResult;

static void
probe_result_clear (gpointer data)
{
  ProbeResult *result = data;

  g_free (result->filename);
}

typedef struct {
  char *probe_name;
  GArray *results;
  double avg;
} ProbeData;

static void
probe_data_free (gpointer data)
{
  ProbeData *p = data;

  g_free (p->probe_name);
  g_array_unref (p->results);

  g_free (p);
}

typedef struct {
  const char *filename;
  GHashTable *probes;
} ForeachClosure;

static gboolean
append_probe (const char *probe_name,
              const char *file,
              const char *function,
              gint32      line,
              gint32      n_samples,
              GVariant   *array,
              gpointer    data)
{
  ForeachClosure *clos = data;

  ProbeData *p = g_hash_table_lookup (clos->probes, probe_name);
  if (p == NULL)
    {
      p = g_new0 (ProbeData, 1);
      p->probe_name = g_strdup (probe_name);
      p->results = g_array_new (FALSE, FALSE, sizeof (ProbeResult));
      p->avg = 0.0;
      g_array_set_clear_func (p->results, probe_result_clear);

      g_hash_table_insert (clos->probes, p->probe_name, p);
    }

  g_autoptr(GArray) samples = g_array_new (FALSE, FALSE, sizeof (ProfileSample));

  GVariantIter iter;
  g_variant_iter_init (&iter, array);

  gint64 start, end;
  while (g_variant_iter_next (&iter, "(xx)", &start, &end))
    {
      g_array_append_vals (samples,
                           &(ProfileSample) {
                             .start_time = start,
                             .end_time = end,
                           },
                           1);
    }

  gint64 min_sample = G_MAXINT64, max_sample = 0;
  gint64 total = 0;

  g_autoptr(GArray) valid_samples = g_array_new (FALSE, FALSE, sizeof (guint));

  for (int i = 0; i < samples->len; i++)
    {
      const ProfileSample *sample = &g_array_index (samples, ProfileSample, i);

      gint64 delta = sample->end_time - sample->start_time;

      /* If the probe never got stopped we need to skip this sample */
      if (delta < 0)
        continue;

      g_array_append_val (valid_samples, i);

      if (delta < min_sample)
        min_sample = delta;
      if (delta > max_sample)
        max_sample = delta;

      total += delta;
    }

  double avg = 0.0;
  
  if (valid_samples->len > 0)
    avg = total / valid_samples->len;

  if (p->avg < avg)
    p->avg = avg;

  g_array_append_vals (p->results,
                       &(ProbeResult) {
                         .filename = g_strdup (clos->filename),
                         .avg = avg,
                       }, 1);

  return TRUE;
}

#define AUTO_FD_INVALID (-1)

typedef int AutoFd;

static void
auto_fd_clear (AutoFd *fd)
{
  if (fd == NULL || *fd == AUTO_FD_INVALID)
    return;

  int rfd = *fd;
  *fd = AUTO_FD_INVALID;
  close (rfd);
}

static int
auto_fd_steal (AutoFd *fd)
{
  if (fd == NULL)
    return AUTO_FD_INVALID;

  int rfd = *fd;
  *fd = AUTO_FD_INVALID;

  return rfd;
}

G_DEFINE_AUTO_CLEANUP_CLEAR_FUNC (AutoFd, auto_fd_clear)

static int
copy_fallback (const char *src,
               const char *dest)
{
  g_auto(AutoFd) src_fd = open (src, O_RDONLY);

  if (src_fd < 0)
    return EXIT_FAILURE;

  g_auto(AutoFd) dest_fd = open (dest, O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (dest_fd < 0)
    return EXIT_FAILURE;

  int res = 0;

  do
    {
      char buf[8192];

      res = read (src_fd, buf, 8192);
      if (res < 0)
        {
          int errno_sv = errno;

          eos_profile_util_print_error ("Unable to read from '%s': %s",
                                        src,
                                        g_strerror (errno_sv));
          return EXIT_FAILURE;
        }

      if (write (dest_fd, buf, res) < 0)
        {
          int errno_sv = errno;

          eos_profile_util_print_error ("Unable to write to '%s': %s",
                                        dest,
                                        g_strerror (errno_sv));
          return EXIT_FAILURE;
        }
    }
  while (res != 0);

  return EXIT_SUCCESS;
}

int
eos_profile_cmd_diff_main (void)
{
  int n_files = g_strv_length (opt_files);
  int i = 0;

  g_autoptr(GHashTable) probes =
    g_hash_table_new_full (g_str_hash, g_str_equal,
                           NULL,
                           probe_data_free);

  while (i < n_files)
    {
      g_autoptr(GError) error = NULL;

      GvdbTable *db = gvdb_table_new (opt_files[i], TRUE, &error);
      if (error != NULL)
        {
          eos_profile_util_print_error ("Unable to load '%s': %s\n",
                                        opt_files[i],
                                        error->message);
          return 1;
        }

      GVariant *v = gvdb_table_get_raw_value (db, PROBE_DB_META_VERSION_KEY);
      gint32 version = v != NULL ? g_variant_get_int32 (v) : -1;
      g_clear_pointer (&v, g_variant_unref);

      if (version != PROBE_DB_VERSION)
        {
          eos_profile_util_print_error ("Unable to load '%s': invalid version\n", opt_files[i]);
          gvdb_table_free (db);
          return 1;
        }

      ForeachClosure clos = { 
        .filename = opt_files[i],
        .probes = probes,
      };

      eos_profile_util_foreach_probe_v1 (db, append_probe, &clos);

      gvdb_table_free (db);

      i += 1;
    }

  g_autoptr(JsonNode) json_res = NULL;
  g_autoptr(GString) buf = NULL;

  if (g_strcmp0 (opt_format, "json") == 0)
    {
      json_res = json_node_new (JSON_NODE_ARRAY);

      JsonArray *arr = json_array_new ();
      json_node_take_array (json_res, arr);
    }
  else if (g_strcmp0 (opt_format, "plain") == 0)
    {
      buf = g_string_new (NULL);
    }

  GHashTableIter iter;
  gpointer value;

  g_hash_table_iter_init (&iter, probes);
  while (g_hash_table_iter_next (&iter, NULL, &value))
    {
      ProbeData *p = value;
      JsonArray *res_array = NULL;
      JsonArray *avg_array = NULL;
      JsonObject *obj = NULL;

      if (json_res != NULL)
        {
          res_array = json_node_get_array (json_res);

          obj = json_object_new ();
          json_array_add_object_element (res_array, obj);

          json_object_set_string_member (obj, "probeName", p->probe_name);

          avg_array = json_array_sized_new (p->results->len);
          json_object_set_array_member (obj, "averageResults", avg_array);
        }
      else
        {
          g_string_append_printf (buf,
                                  "Probe: %s\n"
                                  "  ┕━ • avg: ",
                                  p->probe_name);
        }

      for (int j = 0; j < p->results->len; j++)
        {
          ProbeResult *r = &g_array_index (p->results, ProbeResult, j);

          if (avg_array != NULL)
            {
              json_array_add_double_element (avg_array, r->avg);
            }
          else
            {
              g_string_append_printf (buf, "%.02f %s", scale_val (r->avg), unit_for (r->avg));

              if (fabs (r->avg - p->avg) < FLT_EPSILON)
                g_string_append (buf, "[=]");
              else if (r->avg > p->avg)
                g_string_append (buf, "[+]");
              else if (r->avg < p->avg)
                g_string_append (buf, "[-]");

              if (j == p->results->len - 1)
                g_string_append (buf, "\n");
              else
                g_string_append (buf, ", ");
           }
        }
    }

  g_autofree char *data = NULL;

  if (json_res != NULL)
    {
      g_autoptr(JsonGenerator) gen = json_generator_new ();

      json_generator_set_root (gen, json_res);
      data = json_generator_to_data (gen, NULL);
    }
  else if (buf != NULL)
    {
      data = g_string_free (buf, FALSE);
      buf = NULL;
    }

  write (output_fd, data, strlen (data));
  if (output_fd != STDOUT_FILENO)
    {
      close (output_fd);

      if (rename (output_tmpfile, opt_output) != 0)
        {
          int errno_sv = errno;
          int res = EXIT_SUCCESS;

          /* Fall back to a real copy if the temp file and the real output
           * file are not on the same device
           */
          if (errno_sv == EXDEV)
            res = copy_fallback (output_tmpfile, opt_output);
          else
            {
              eos_profile_util_print_error ("Unable to save output to '%s': %s",
                                            opt_output,
                                            g_strerror (errno_sv));
              res = EXIT_FAILURE;
            }

          unlink (output_tmpfile);

          return res;
        }
    }
  else
    write (output_fd, "\n", 1);

  return EXIT_SUCCESS;
}
