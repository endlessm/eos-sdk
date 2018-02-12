#pragma once

#include <glib.h>

#include "endless/gvdb/gvdb-reader.h"

typedef enum {
  EOS_PRINT_COLOR_GREEN,
  EOS_PRINT_COLOR_BLUE,
  EOS_PRINT_COLOR_YELLOW,
  EOS_PRINT_COLOR_RED,

  EOS_PRINT_COLOR_NONE
} EosPrintColor;

void    eos_profile_util_print_message  (const char *prefix,
                                         EosPrintColor color,
                                         const char *fmt,
                                         ...) G_GNUC_PRINTF (3, 4);

void    eos_profile_util_print_error    (const char *msg,
                                         ...) G_GNUC_PRINTF (1, 2);

void    eos_profile_util_print_warning  (const char *msg,
                                         ...) G_GNUC_PRINTF (1, 2);

typedef gboolean (* EosProfileProbeCallback) (const char *probe_name,
                                              const char *function,
                                              const char *file,
                                              gint32      line,
                                              gint32      n_samples,
                                              GVariant   *samples,
                                              gpointer    user_data);

void    eos_profile_util_foreach_probe_v1       (GvdbTable               *db,
                                                 EosProfileProbeCallback  callback,
                                                 gpointer                 callback_data);
