#include "config.h"

#include "eos-profile-cmds.h"
#include "eos-profile-utils.h"

#include "endless/eosprofile-private.h"
#include "endless/gvdb/gvdb-reader.h"

#include <math.h>

static GPtrArray *files;

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

gboolean
eos_profile_cmd_show_parse_args (int    argc,
                                 char **argv)
{
  files = g_ptr_array_new ();

  for (int i = 1; i < argc; i++)
    g_ptr_array_add (files, argv[i]);

  if (files->len == 0)
    {
      g_printerr ("Usage: eos-profile show FILE [FILE...]\n");
      g_ptr_array_unref (files);

      return FALSE;
    }

  return TRUE;
}

static void
print_probe (const char *name)
{
  eos_profile_util_print_message ("PROBE", EOS_PRINT_COLOR_GREEN,
                                  "%s",
                                  name);
}

static void
print_location (const char *file,
                gint32      line,
                const char *function)
{
  eos_profile_util_print_message (NULL, EOS_PRINT_COLOR_NONE,
                                  " `- %s at %s:%d",
                                  function,
                                  file,
                                  line);
}

static void
print_samples (const char *name,
               gint32      n_samples,
               GVariant   *array)
{
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

  g_autofree char *msg = NULL;

  if (valid_samples->len > 1)
    {
      double avg = total / (double) valid_samples->len;
      double s = 0;
      double s_part = 0;

      for (int i = 1; i < valid_samples->len - 1; i++)
        {
          guint idx = g_array_index (valid_samples, guint, i);
          const ProfileSample *sample = &g_array_index (samples, ProfileSample, idx);

          gint64 delta = sample->end_time - sample->start_time;
          g_assert (delta >= 0);

          double deviation = delta - avg;
          s_part += (deviation * deviation);
        }

      if (valid_samples->len > 1)
        s = sqrt (s_part / (double) valid_samples->len - 1);
      else
        s = 0.0;

      g_autofree char *stddev = g_strdup_printf (", σ: %g", s);

      msg =
        g_strdup_printf ("%d samples: total time: %d %s, avg: %g %s, min: %d %s, max: %d %s%s",
                         valid_samples->len,
                         (int) scale_val (total), unit_for (total),
                         scale_val (avg), unit_for (avg),
                         (int) scale_val (min_sample), unit_for (min_sample),
                         (int) scale_val (max_sample), unit_for (max_sample),
                         s == 0.0 ? "" : stddev);
    }
  else if (valid_samples->len == 1)
    {
      msg = g_strdup_printf ("1 sample: total time: %d %s",
                             (int) scale_val (total),
                             unit_for (total));
    }
  else
    {
      msg = g_strdup ("Not enough valid samples found");
    }

  eos_profile_util_print_message (NULL, EOS_PRINT_COLOR_NONE,
                                  " `- %s",
                                  msg);
}

int
eos_profile_cmd_show_main (void)
{
  g_assert (files != NULL);

  for (int i = 0; i < files->len; i++)
    {
      const char *filename = g_ptr_array_index (files, i);
      g_autoptr(GError) error = NULL;

      eos_profile_util_print_message ("INFO", EOS_PRINT_COLOR_BLUE,
                                      "Loading profiling data from '%s'",
                                      filename);

      GvdbTable *db = gvdb_table_new (filename, TRUE, &error);

      if (error != NULL)
        {
          eos_profile_util_print_error ("Unable to load '%s': %s\n", filename, error->message);
          return 1;
        }

      g_autoptr(GVariant) v = gvdb_table_get_raw_value (db, PROBE_DB_META_VERSION_KEY);
      gint32 version = v != NULL ? g_variant_get_int32 (v) : -1;

      if (version != PROBE_DB_VERSION)
        {
          eos_profile_util_print_error ("Unable to load '%s': invalid version\n");
          return 1;
        }

      int names_len = 0;
      g_auto(GStrv) names = gvdb_table_get_names (db, &names_len);

      for (int j = 0; j < names_len; j++)
        {
          const char *name = names[j];

          if (g_strcmp0 (name, PROBE_DB_META_VERSION_KEY) == 0)
            continue;

          g_autoptr(GVariant) value = gvdb_table_get_raw_value (db, name);
          if (value == NULL)
            continue;

          if (g_strcmp0 (name, PROBE_DB_META_APPID_KEY) == 0)
            {
              const char *appid = g_variant_get_string (value, NULL);

              eos_profile_util_print_message ("INFO", EOS_PRINT_COLOR_BLUE,
                                              "Application: %s",
                                              appid);
              continue;
            }

          if (g_strcmp0 (name, PROBE_DB_META_PROFILE_KEY) == 0)
            {
              gint64 profile_time = g_variant_get_int64 (value);

              eos_profile_util_print_message ("INFO", EOS_PRINT_COLOR_BLUE,
                                              "Profile time: %d %s",
                                              (int) scale_val (profile_time),
                                              unit_for (profile_time));
              continue;
            }

          const char *file = NULL;
          const char *function = NULL;
          const char *probe_name = NULL;
          g_autoptr(GVariant) samples = NULL;
          gint32 line, n_samples;

          g_variant_get (value, "(&s&s&suu@a(xx))",
                         &probe_name,
                         &function,
                         &file,
                         &line,
                         &n_samples,
                         &samples);

          print_probe (probe_name);
          print_location (file, line, function);
          if (n_samples > 0)
            print_samples (probe_name, n_samples, samples);
          
        }

      gvdb_table_free (db);
    }

  return 0;
}
