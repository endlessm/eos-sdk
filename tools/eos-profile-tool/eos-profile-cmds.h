#pragma once

#include <glib.h>

typedef gboolean (* EosProfileCmdParseArgs) (int argc, char **argv);
typedef int (* EosProfileCmdMain) (void);

gboolean        eos_profile_cmd_help_parse_args (int argc, char **argv);
int             eos_profile_cmd_help_main       (void);

const struct {
  const char *name;
  const char *description;

  EosProfileCmdParseArgs parse_args;
  EosProfileCmdMain main;
} profile_commands[] = {
  {
    .name = "help",
    .description = "Prints help",
    .parse_args = eos_profile_cmd_help_parse_args,
    .main = eos_profile_cmd_help_main,
  },

  { NULL, },
};
