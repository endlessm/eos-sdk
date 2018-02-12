#include "config.h"

#include "eos-profile-cmds.h"
#include "eos-profile-utils.h"

#include "endless/eosprofile-private.h"
#include "endless/gvdb/gvdb-reader.h"

#include <math.h>
#include <json-glib/json-glib.h>

static char *opt_format;
static char *opt_output;
static char *opt_input;
static gboolean opt_pretty;

static GOptionEntry opts[] = {
  {
    .long_name = "format",
    .short_name = 'f',
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_STRING,
    .arg_data = &opt_format,
    .description = "The output format (valid values: json)",
    .arg_description = "FORMAT",
  },
  {
    .long_name = "pretty",
    .short_name = 0,
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_NONE,
    .arg_data = &opt_pretty,
    .description = "Enable pretty-printing the output",
    .arg_description = NULL,
  },
  {
    .long_name = "output",
    .short_name = 0,
    .flags = G_OPTION_FLAG_NONE,
    .arg = G_OPTION_ARG_FILENAME,
    .arg_data = &opt_output,
    .description = "The output file",
    .arg_description = "FILE",
  },

  { NULL, },
};

gboolean
eos_profile_cmd_convert_parse_args (int    argc,
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

  if (argc < 2)
    return FALSE;

  opt_input = argv[1];

  return TRUE;
}

static JsonNode *
collect_probe_samples (GVariant   *array,
                       JsonObject *probe_obj)
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

  if (valid_samples->len > 0)
    {
      JsonObject *obj = json_object_new ();

      double avg = total / (double) valid_samples->len;
      double s = 0;
      double s_part = 0;

      JsonArray *raw_array = json_array_sized_new (valid_samples->len - 1);

      for (int i = 1; i < valid_samples->len - 1; i++)
        {
          guint idx = g_array_index (valid_samples, guint, i);
          const ProfileSample *sample = &g_array_index (samples, ProfileSample, idx);

          gint64 delta = sample->end_time - sample->start_time;
          g_assert (delta >= 0);

          double deviation = delta - avg;
          s_part += (deviation * deviation);

          json_array_add_int_element (raw_array, delta);
        }

      json_object_set_int_member (probe_obj, "numSamples", valid_samples->len - 1);
      json_object_set_array_member (probe_obj, "rawSamples", raw_array);

      if (valid_samples->len > 1)
        s = sqrt (s_part / (double) valid_samples->len - 1);
      else
        s = 0.0;

      if (s != 0.0 && !isnan (s))
        json_object_set_double_member (probe_obj, "sigma", s);

      json_object_set_int_member (probe_obj, "totalTime", total);

      if (valid_samples->len > 1)
        {
          json_object_set_double_member (probe_obj, "minSample", min_sample);
          json_object_set_double_member (probe_obj, "maxSample", max_sample);
          json_object_set_double_member (probe_obj, "average", avg);
        }
    }
  else
    {
      json_object_set_int_member (probe_obj, "totalTime", total);
      json_object_set_int_member (probe_obj, "numSamples", 0);
      json_object_set_array_member (probe_obj, "rawSamples", NULL);
    }
}

static gboolean
append_probe (const char *probe_name,
              const char *file,
              const char *function,
              gint32      line,
              gint32      n_samples,
              GVariant   *samples,
              gpointer    data)
{
  JsonArray *probes_arr = data;

  JsonObject *probe_obj = json_object_new ();

  json_object_set_string_member (probe_obj, "name", probe_name);
  json_object_set_string_member (probe_obj, "file", file);
  json_object_set_int_member (probe_obj, "line", line);
  json_object_set_string_member (probe_obj, "function", function);

  JsonObject *samples_obj = json_object_new ();
  collect_probe_samples (samples, samples_obj);
  json_object_set_object_member (probe_obj, "samples", samples_obj);

  json_array_add_object_element (probes_arr, probe_obj);

  return TRUE;
}


int
eos_profile_cmd_convert_main (void)
{
  g_assert (opt_input != NULL);

  if (opt_format == NULL)
    opt_format = "json";

  if (g_strcmp0 (opt_format, "json") != 0)
    {
      eos_profile_util_print_error ("Unknown format '%s'; please, use 'json'", opt_format);
      return 1;
    }

  g_autoptr(GError) error = NULL;

  GvdbTable *db = gvdb_table_new (opt_input, TRUE, &error);
  if (error != NULL)
    {
      eos_profile_util_print_error ("Unable to load '%s': %s\n",
                                    opt_input,
                                    error->message);
      return 1;
    }

  GVariant *v = gvdb_table_get_raw_value (db, PROBE_DB_META_VERSION_KEY);
  gint32 version = v != NULL ? g_variant_get_int32 (v) : -1;
  g_clear_pointer (&v, g_variant_unref);

  if (version != PROBE_DB_VERSION)
    {
      eos_profile_util_print_error ("Unable to load '%s': invalid version\n", opt_input);
      gvdb_table_free (db);
      return 1;
    }

  v = gvdb_table_get_raw_value (db, PROBE_DB_META_APPID_KEY);
  g_autofree char *appid = v != NULL ? g_variant_dup_string (v, NULL) : NULL;
  g_clear_pointer (&v, g_variant_unref);

  v = gvdb_table_get_raw_value (db, PROBE_DB_META_PROFILE_KEY);
  gint64 profile_time = v != NULL ? g_variant_get_int64 (v) : -1;
  g_clear_pointer (&v, g_variant_unref);

  v = gvdb_table_get_raw_value (db, PROBE_DB_META_START_KEY);
  g_autofree char *start_time = NULL;
  if (v != NULL)
    {
      g_autoptr(GDateTime) dt =
        g_date_time_new_from_unix_local (g_variant_get_int64 (v));

        start_time = g_date_time_format (dt, "%Y-%m-%d %T");
    }
  g_clear_pointer (&v, g_variant_unref);

  JsonNode *root = json_node_new (JSON_NODE_OBJECT);
  JsonObject *obj = json_object_new ();
  json_node_take_object (root, obj);

  JsonObject *meta = json_object_new ();
  json_object_set_int_member (meta, "version", version);
  json_object_set_string_member (meta, "appId", appid);
  json_object_set_int_member (meta, "profileTime", profile_time);
  json_object_set_string_member (meta, "startTime", start_time);

  json_object_set_object_member (obj, "meta", meta);

  JsonArray *probes_arr = json_array_new ();

  eos_profile_util_foreach_probe_v1 (db, append_probe, probes_arr);

  json_object_set_array_member (obj, "probes", probes_arr);

  gvdb_table_free (db);

  g_autoptr(JsonGenerator) gen = json_generator_new ();
  json_generator_set_pretty (gen, opt_pretty);
  json_generator_set_root (gen, root);

  if (opt_output != NULL)
    {
      g_autoptr(GError) gen_error = NULL;

      json_generator_to_file (gen, opt_output, &error);
      if (error != NULL)
        {
          eos_profile_util_print_error ("Unable to write to '%s': %s",
                                        opt_output,
                                        error->message);
          return 1;
        }
    }
  else
    {
      g_autofree char *str = json_generator_to_data (gen, NULL);

      g_print ("%s\n", str);
    }

  return 0;
}
