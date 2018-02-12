#include "config.h"

#include "eos-profile-utils.h"

#include "endless/eosprofile-private.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <math.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

static const char *ansi_colors[] = {
  [EOS_PRINT_COLOR_GREEN] = "[1;32m",
  [EOS_PRINT_COLOR_BLUE] = "[1;34m",
  [EOS_PRINT_COLOR_YELLOW] = "[1;33m",
  [EOS_PRINT_COLOR_RED] = "[1;31m",
  [EOS_PRINT_COLOR_NONE] = "[0m",
};

static char *
gen_color_message (const char *prefix,
                   EosPrintColor color,
                   const char *fmt,
                   va_list args)
{
  g_autofree char *res = NULL;

  if (prefix != NULL)
    {
      g_autofree char *msg = g_strdup_vprintf (fmt, args);

      res = g_strdup_printf ("\033%s%s\033%s: %s",
                             ansi_colors[color],
                             prefix,
                             ansi_colors[EOS_PRINT_COLOR_NONE],
                             msg);

    }
  else
    res = g_strdup_vprintf (fmt, args);

  return g_steal_pointer (&res);
}

void
eos_profile_util_print_message (const char *prefix,
                                EosPrintColor color,
                                const char *fmt,
                                ...)
{
  g_autofree char *msg = NULL;
  va_list args;

  va_start (args, fmt);
  msg = gen_color_message (prefix, color, fmt, args);
  va_end (args);

  g_print ("%s\n", msg);
}

void
eos_profile_util_print_error (const char *fmt,
                              ...)
{
  g_autofree char *msg = NULL;
  va_list args;

  va_start (args, fmt);
  msg = gen_color_message ("ERROR", EOS_PRINT_COLOR_RED, fmt, args);
  va_end (args);

  g_printerr ("%s\n", msg);
}

void
eos_profile_util_print_warning (const char *fmt,
                                ...)
{
  g_autofree char *msg = NULL;
  va_list args;

  va_start (args, fmt);
  msg = gen_color_message ("WARNING", EOS_PRINT_COLOR_YELLOW, fmt, args);
  va_end (args);

  g_printerr ("%s\n", msg);
}

void
eos_profile_util_foreach_probe_v1 (GvdbTable               *db,
                                   EosProfileProbeCallback  callback,
                                   gpointer                 callback_data)
{
  int names_len = 0;
  g_auto(GStrv) names = gvdb_table_get_names (db, &names_len);

  const char * const meta_keys[] = {
    PROBE_DB_META_VERSION_KEY,
    PROBE_DB_META_APPID_KEY,
    PROBE_DB_META_PROFILE_KEY,
    PROBE_DB_META_START_KEY,
    NULL,
  };

  for (int i = 0; i < names_len; i++)
    {
      const char *key_name = names[i];

      if (g_strv_contains (meta_keys, key_name))
        continue;

      if (!gvdb_table_has_value (db, key_name))
        continue;

      g_autoptr(GVariant) value = gvdb_table_get_raw_value (db, key_name);
      if (value == NULL)
        continue;

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

      if (!callback (probe_name, function, file, line, n_samples, samples, callback_data))
        break;
   }
}
