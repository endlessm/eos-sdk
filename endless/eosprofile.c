/* Copyright 2017 Endless Mobile, Inc. */

#include "config.h"

#include "eosprofile-private.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>

#include "gvdb/gvdb-builder.h"

/**
 * SECTION:profiling
 * @Title: Profiling
 * @Short_description: Profiling tools for applications
 *
 * The profiling API provided by the Endless SDK is a simple tool for defining
 * profiling probes and collecting data over multiple calls.
 *
 * ### Enabling profiling
 *
 * Profile probes try to be as close to zero-cost as possible; they are only
 * enabled if the `EOS_PROFILE` environment variable is set. This means that
 * you can leave the profile probes in your code, and they will be inert until
 * the environment is set up for profiling.
 *
 * ### Using profiling probes
 *
 * Typically, you want to declare a profiling probe at the beginning of the
 * section of the code you wish to measure, and stop it at the end.
 *
 * The profiling probes are identified by a unique name, typically expressed
 * as a path, like `/com/example/ExampleProbe`; this allows creating a tree of
 * probes.
 *
 * If you are using the C API and a GCC-compatible compiler, you will want to
 * use the `g_autoptr()` macro to declare the profiling probe, and have it
 * automatically collected at the end of the scope, for instance:
 *
 * |[<!-- language="C" -->
 * static void
 * some_function (SomeObject *obj)
 * {
 *   some_set_up (obj);
 *
 *   // Here begins the section we wish to profile
 *   g_autoptr(EosProfileProbe) outer = EOS_PROFILE ("/com/example/some-function");
 *
 *   some_expensive_computation (obj);
 *   some_additional_work (obj);
 *
 *   if (some_state (obj))
 *     {
 *       g_autoptr(EosProfileProbe) inner = EOS_PROFILE ("/com/example/some-function/state");
 *
 *       some_more_computation (obj);
 *     }
 * }
 * ]|
 *
 * In the example above, the `outer` probe is created after we performed some
 * operation; since we are not interested into its cost, we are going to
 * ignore it. Additionally, the `inner` probe is created conditionally on some
 * state, so we can also gather information on the actual number of times the
 * inner function is called. In either cases, both the `outer` and `inner`
 * probes are automatically stopped once they get out of scope.
 *
 * ### Capturing profiling data
 *
 * By default, when the `EOS_PROFILE` environment variable is set, you will
 * get a summary at the end of the process, sent to the standard output.
 *
 * It is also possible to redirect the profiling data to a capture file, by
 * setting the `EOS_PROFILE` environment variable to the `capture` value. In
 * that case, the profiling data will be stored in a binary format at the
 * end of the process, and you can use the `eos-profile` tool to extract the
 * probes, timings, and generate a summary. The default filename for the
 * captured data is based on the name of the binary and the process ID, and
 * it's saved under the `$XDG_CACHE_HOME` directory (see: g_get_user_cache_dir()).
 *
 * You can also specify the name of the capture file, by setting the
 * `EOS_PROFILE` environment variable to `capture:/path/to/file`.
 */

static int
sample_compare (gconstpointer a,
                gconstpointer b)
{
  const ProfileSample *sample_a = a;
  const ProfileSample *sample_b = b;

  /* Times are monotonic, so this is always positive */
  gint64 delta_a = sample_a->end_time - sample_a->start_time;
  gint64 delta_b = sample_b->end_time - sample_b->start_time;

  if (delta_a < delta_b)
    return -1;

  if (delta_a > delta_b)
    return 1;

  return 0;
}

#define N_SAMPLES       64

struct _EosProfileProbe {
  volatile int ref_count;

  char *file;
  gint32 line;
  char *function;
  char *name;

  GArray *samples;

  GMutex probe_lock;
};

static EosProfileProbe eos_profile_dummy_probe;

static EosProfileProbe *
eos_profile_probe_new (const char *file,
                       gsize       line,
                       const char *function,
                       const char *name)
{
  EosProfileProbe *res = g_new0 (EosProfileProbe, 1);

  res->ref_count = 1;

  res->name = g_strdup (name);
  res->function = g_strdup (function);
  res->file = g_strdup (file);
  res->line = line;

  res->samples = g_array_sized_new (FALSE, FALSE, sizeof (ProfileSample), N_SAMPLES);

  g_mutex_init (&res->probe_lock);

  return res;
}

static void
eos_profile_probe_destroy (gpointer data)
{
  EosProfileProbe *probe = data;

  g_hash_table_remove (profile_state->probes, probe->name);

  g_array_unref (probe->samples);
  g_free (probe->name);
  g_free (probe->function);
  g_free (probe->file);

  g_free (probe);
}

static EosProfileProbe *
eos_profile_probe_copy (EosProfileProbe *probe)
{
  return probe;
}

static void
eos_profile_probe_free (EosProfileProbe *probe)
{
  /* no-op */
}

G_DEFINE_BOXED_TYPE (EosProfileProbe, eos_profile_probe,
                     eos_profile_probe_copy,
                     eos_profile_probe_free)

/**
 * eos_profile_probe_start:
 * @file: the source file for the probe, typically represented by %__FILE__
 * @line: the line in the source @file, typically represented by %__LINE__
 * @function: the function for the probe, typically represented by %G_STRFUNC
 * @name: a unique name for the probe
 *
 * Starts a profiling probe for @name, creating it if necessary.
 *
 * Returns: (transfer none): a profile probe identifier; use eos_profile_probe_stop()
 *   to stop the profiling on the returned probe
 *
 * Since: 0.6
 */
EosProfileProbe *
eos_profile_probe_start (const char *file,
                         gsize       line,
                         const char *function,
                         const char *name)
{
  /* Don't measure the lock */
  gint64 sample_time = g_get_monotonic_time ();

  /* We can take this out of the lock because by the time we reach
   * eos_profile_probe_stop() the profile state is guaranteed to
   * either always exist, or not
   */
  if (profile_state == NULL)
    return &eos_profile_dummy_probe;

  G_LOCK (profile_state);

  EosProfileProbe *res = g_hash_table_lookup (profile_state->probes, name);
  if (res == NULL)
    {
      res = eos_profile_probe_new (file, line, function, name);

      g_hash_table_insert (profile_state->probes, res->name, res);
    }

  g_array_append_vals (res->samples,
                       &(ProfileSample) {
                         .start_time = sample_time,
                         .end_time = -1
                       },
                       1);

  G_UNLOCK (profile_state);

  return (EosProfileProbe *) res;
}

/**
 * eos_profile_probe_stop:
 * @probe: a #EosProfileProbe
 *
 * Stops a profiling probe started using eos_profile_probe_start().
 *
 * Since: 0.6
 */
void
eos_profile_probe_stop (EosProfileProbe *probe)
{
  if (probe == &eos_profile_dummy_probe)
    return;

  /* Don't measure the lock */
  gint64 sample_time = g_get_monotonic_time ();

  g_autoptr(GMutexLocker) locker = g_mutex_locker_new (&probe->probe_lock);

  /* Ideally, we just want to update the sample we just created, which means
   * picking the last slot in the samples array; in practice, this is what
   * should happen most of the time, unless we end up recursing. In that case
   * we end up with creating a sample while the outer sample is still in
   * flight.
   *
   * If we just recorded the inner samples, we'd end up with a skewed capture,
   * as those samples would inevitably be masking the timing of the outer
   * samples.
   *
   * The easiest approach is to discard the inner samples until we reach the
   * outermost live sample
   */
  int first_in_flight = probe->samples->len - 1;
  for (int i = probe->samples->len - 1; i >= 0; i--)
    {
      const ProfileSample *sample = &g_array_index (probe->samples, ProfileSample, i);

      if (sample->end_time > 0)
        break;

      first_in_flight = i;
    }

  ProfileSample *sample = &g_array_index (probe->samples, ProfileSample, first_in_flight);

  sample->end_time = sample_time;

  if (first_in_flight != probe->samples->len - 1)
    {
      int range = probe->samples->len - first_in_flight - 1;

      g_array_remove_range (probe->samples, first_in_flight + 1, range);
    }
}

void
eos_profile_state_init (void)
{
  static gboolean profile_state_inited;

  if (profile_state_inited)
    return;

  profile_state_inited = TRUE;

  const char *str = getenv ("EOS_PROFILE");
  if (str != NULL)
    {
      profile_state = g_new0 (ProfileState, 1);
      profile_state->probes = g_hash_table_new_full (g_str_hash, g_str_equal,
                                                     NULL,
                                                     eos_profile_probe_destroy);

      int capture_prefix_len = strlen ("capture");

      if (g_ascii_strncasecmp (str, "capture", capture_prefix_len) == 0)
        {
          profile_state->capture = TRUE;

          const char *filename = str + capture_prefix_len;
          if (*filename == ':')
            {
              filename += 1;

              if (*filename != '\0')
                profile_state->capture_file = g_strdup (filename);
            }

          if (profile_state->capture_file == NULL || *profile_state->capture_file == '\0')
            {
              g_autofree char *capture_dir = g_build_filename (g_get_user_cache_dir (),
                                                               "com.endlessm.Sdk.Profile",
                                                               NULL);

              if (g_mkdir_with_parents (capture_dir, 0700) < 0)
                capture_dir = g_get_current_dir ();

              profile_state->capture_file = g_strdup_printf ("%s%s%s.db",
                                                             capture_dir,
                                                             G_DIR_SEPARATOR_S,
                                                             g_get_prgname ());
            }
        }
    }
}

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

static void
profile_state_dump_to_console (void)
{
  gushort max_columns = 256;

  if (isatty (STDOUT_FILENO))
    {
      struct winsize w;

      ioctl (STDOUT_FILENO, TIOCGWINSZ, &w);

      max_columns = w.ws_col;
    }

  GHashTableIter iter;
  gpointer value;

  g_hash_table_iter_init (&iter, profile_state->probes);
  while (g_hash_table_iter_next (&iter, NULL, &value))
    {
      EosProfileProbe *probe = value;

      /* Take ownership of the samples in order to sort them; we want to
       * pre-sort so that we can easily discard the outliers when doing
       * our analysis, later on
       */
      GArray *sorted_samples = g_steal_pointer (&probe->samples);
      g_array_sort (sorted_samples, sample_compare);

      gint64 min_sample = G_MAXINT64, max_sample = 0;
      gint64 total = 0;

      g_autoptr(GArray) valid_samples = g_array_new (FALSE, FALSE, sizeof (guint));

      for (int i = 0; i < sorted_samples->len; i++)
        {
          const ProfileSample *sample = &g_array_index (sorted_samples, ProfileSample, i);

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
              const ProfileSample *sample = &g_array_index (sorted_samples, ProfileSample, idx);

              gint64 delta = sample->end_time - sample->start_time;
              g_assert (delta >= 0);

              double deviation = delta - avg;
              s_part += (deviation * deviation);
            }

          if (valid_samples->len > 1)
            s = sqrt (s_part / (double) valid_samples->len - 1);
          else
            s = 0.0;

          g_autofree char *stddev = g_strdup_printf (", σ:%g", s);

          msg =
            g_strdup_printf ("%d samples: avg:%g %s, min:%d %s, max:%d %s%s)",
                             valid_samples->len,
                             scale_val (avg), unit_for (avg),
                             (int) scale_val (min_sample), unit_for (min_sample),
                             (int) scale_val (max_sample), unit_for (max_sample),
                             s == 0.0 ? "" : stddev);
        }
      else if (valid_samples->len == 1)
        {
          msg = g_strdup_printf ("total time:%d %s",
                                 (int) scale_val (total),
                                 unit_for (total));
        }
      else
        {
          msg = g_strdup ("not enough valid samples found");
        }

      g_autofree char *probe_name = NULL;

      int probe_len = strlen (probe->name);
      int msg_len = strlen (msg);
      if (probe_len + msg_len >= (int) max_columns - 2)
        {
          gsize name_len = MAX ((int) max_columns - msg_len, 2);
          probe_name = g_strndup (probe->name, name_len);
          probe_name[name_len - 1] = '~';
        }
      else
        probe_name = g_strdup (probe->name);

      g_print ("%s%*c%s\n",
               probe_name,
               max_columns - strlen (probe_name) - msg_len, ' ',
               msg);
      g_print ("  %s at %s:%d\n\n",
               probe->function,
               probe->file, probe->line);
    }
}

/* Get the immediate parent table in the GVDB table, using the
 * key separator '/' to determine the nesting level. If needed,
 * this function will create the intermediate tables
 */
static GvdbItem *
get_parent (GHashTable *table,
            char       *key,
            int         length)
{
  GvdbItem *grandparent, *parent;

  if (length == 1)
    return NULL;

  while (key[--length - 1] != '/')
    ;

  key[length] = '\0';

  parent = g_hash_table_lookup (table, key);

  if (parent == NULL)
    {
      parent = gvdb_hash_table_insert (table, key);

      grandparent = get_parent (table, key, length);

      if (grandparent != NULL)
        gvdb_item_set_parent (parent, grandparent);
    }

  return parent;
}

void
eos_profile_state_dump (void)
{
  if (profile_state == NULL)
    return;

  if (!profile_state->capture)
    {
      profile_state_dump_to_console ();
      return;
    }

  g_autoptr(GHashTable) db_table = gvdb_hash_table_new (NULL, NULL);

  /* Metadata for the DB */
  g_autofree char *version_key = g_strdup (PROBE_DB_META_VERSION_KEY);
  gsize version_key_len = strlen (version_key);
  GvdbItem *meta = gvdb_hash_table_insert (db_table, PROBE_DB_META_VERSION_KEY);
  gvdb_item_set_parent (meta, get_parent (db_table, version_key, version_key_len));
  gvdb_item_set_value (meta, g_variant_new_int32 (PROBE_DB_VERSION));

  /* Iterate over the probes */
  GHashTableIter iter;
  gpointer value;
  g_hash_table_iter_init (&iter, profile_state->probes);
  while (g_hash_table_iter_next (&iter, NULL, &value))
    {
      EosProfileProbe *probe = value;

      g_autofree char *key = g_strdup (probe->name);
      gsize key_len = strlen (probe->name);

      GvdbItem *item = gvdb_hash_table_insert (db_table, key);
      gvdb_item_set_parent (item, get_parent (db_table, key, key_len));

      GVariantBuilder builder;

      g_variant_builder_init (&builder, G_VARIANT_TYPE ("(sssuua(xx))"));

      g_variant_builder_add (&builder, "s", probe->name);
      g_variant_builder_add (&builder, "s", probe->function);
      g_variant_builder_add (&builder, "s", probe->file);
      g_variant_builder_add (&builder, "u", probe->line);

      g_variant_builder_add (&builder, "u", probe->samples->len);

      /* Take ownership of the samples in order to sort them; we want to
       * pre-sort so that we can easily discard the outliers when doing
       * our analysis, later on
       */
      GArray *sorted_samples = g_steal_pointer (&(probe->samples));
      g_array_sort (sorted_samples, sample_compare);

      g_variant_builder_open (&builder, G_VARIANT_TYPE ("a(xx)"));

      for (int i = 0; i < sorted_samples->len; i++)
        {
          const ProfileSample *sample = &g_array_index (sorted_samples, ProfileSample, i);

          g_variant_builder_open (&builder, G_VARIANT_TYPE ("(xx)"));
          g_variant_builder_add (&builder, "x", sample->start_time);
          g_variant_builder_add (&builder, "x", sample->end_time);
          g_variant_builder_close (&builder);
        }

      g_variant_builder_close (&builder);

      g_array_free (sorted_samples, TRUE);

      gvdb_item_set_value (item, g_variant_builder_end (&builder));
    }

  /* Clean up */
  g_hash_table_unref (profile_state->probes);
  g_free (profile_state->capture_file);
  g_free (profile_state);

  g_autoptr(GError) error = NULL;
  gvdb_table_write_contents (db_table, profile_state->capture_file,
                             G_BYTE_ORDER != G_LITTLE_ENDIAN,
                             &error);

  if (error != NULL)
    g_printerr ("PROFILE: %s\n", error->message);
}
