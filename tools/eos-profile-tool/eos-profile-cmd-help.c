#include "config.h"

#include <glib.h>

gboolean
eos_profile_cmd_help_parse_args (int    argc,
                                 char **argv)
{
  return TRUE;
}

int
eos_profile_cmd_help_main (void)
{
  g_print (
    "eos-profile\n"
    "\n"
    "Usage: eos-profile <COMMAND> [OPTIONS...]\n"
    "\n"
    "Examples:\n"
    "\n"
    "  eos-profile help         - This help screen\n"
    "\n"
  );

  return 0;
}
