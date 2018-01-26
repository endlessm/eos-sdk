#include "config.h"

#include "eos-profile-cmds.h"

gboolean
eos_profile_cmd_help_parse_args (int    argc,
                                 char **argv)
{
  return TRUE;
}

static gboolean
print_available_commands (const EosProfileCmd *cmd,
                          gpointer             dummy G_GNUC_UNUSED)
{
  g_print ("  %s%*s%s\n",
           cmd->name,
           14 - strlen (cmd->name), " ",
           cmd->description);

  return FALSE;
}

int
eos_profile_cmd_help_main (void)
{
  g_print (
    "eos-profile\n"
    "\n"
    "Usage: eos-profile <COMMAND> [OPTION…]\n"
    "\n"
    "COMMANDS\n"
    "\n"
  );

  eos_profile_foreach_cmd (print_available_commands, NULL);

  g_print ("\n");

  return 0;
}
