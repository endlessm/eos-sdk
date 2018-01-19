#pragma once

#include <glib.h>

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
