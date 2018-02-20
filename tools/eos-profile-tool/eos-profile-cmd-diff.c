#include "config.h"

#include "eos-profile-cmds.h"
#include "eos-profile-utils.h"

#include "endless/eosprofile-private.h"
#include "endless/gvdb/gvdb-reader.h"

#include <math.h>

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

static GOptionEntry opts[] = {
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

  GHashTableIter iter;
  gpointer value;

  g_hash_table_iter_init (&iter, probes);
  while (g_hash_table_iter_next (&iter, NULL, &value))
    {
      ProbeData *p = value;

      g_print ("Probe: %s\n", p->probe_name);

      g_print ("  ┕━ • avg: ");
      for (int j = 0; j < p->results->len; j++)
        {
          ProbeResult *r = &g_array_index (p->results, ProbeResult, j);

          g_print ("%.02f %s", scale_val (r->avg), unit_for (r->avg));

          if (r->avg > p->avg)
            g_print ("[+]");
          else if (r->avg < p->avg)
            g_print ("[-]");
          else if (fabs (r->avg - p->avg) < 0.0001)
            g_print ("[~]");
          else
            g_print ("[=]");

          if (j == p->results->len - 1)
            g_print ("\n");
          else
            g_print (", ");
        }
    }

  return 0;
}
