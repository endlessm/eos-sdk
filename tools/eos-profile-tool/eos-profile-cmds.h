#pragma once

#include <glib.h>

typedef gboolean (* EosProfileCmdParseArgs) (int argc, char **argv);
typedef int (* EosProfileCmdMain) (void);

gboolean        eos_profile_cmd_help_parse_args (int argc, char **argv);
int             eos_profile_cmd_help_main       (void);

gboolean        eos_profile_cmd_show_parse_args (int argc, char **argv);
int             eos_profile_cmd_show_main       (void);

