#pragma once

#include <glib.h>

typedef gboolean (* EosProfileCmdParseArgs) (int argc, char **argv);
typedef int (* EosProfileCmdMain) (void);

typedef struct {
  const char *name;
  const char *description;
  const char *usage;

  EosProfileCmdParseArgs parse_args;
  EosProfileCmdMain main;
} EosProfileCmd;

typedef gboolean (* EosProfileCmdCallback) (const EosProfileCmd *cmd,
                                            gpointer             data);

gboolean        eos_profile_cmd_help_parse_args         (int argc, char **argv);
int             eos_profile_cmd_help_main               (void);

gboolean        eos_profile_cmd_show_parse_args         (int argc, char **argv);
int             eos_profile_cmd_show_main               (void);

gboolean        eos_profile_cmd_convert_parse_args      (int argc, char **argv);
int             eos_profile_cmd_convert_main            (void);

void            eos_profile_foreach_cmd         (EosProfileCmdCallback cb,
                                                 gpointer              data);
