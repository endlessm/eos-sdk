#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "eos-profile-cmds.h"

static const struct {
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
  {
    .name = "show",
    .description = "Shows a capture",
    .parse_args = eos_profile_cmd_show_parse_args,
    .main = eos_profile_cmd_show_main,
  },

  { NULL, },
};

int
main (int argc,
      char *argv[])
{
  setlocale (LC_ALL, "");
  textdomain (GETTEXT_PACKAGE);

  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

  const char *cmd = NULL;

  if (argc < 2)
    cmd = "help";
  else
    cmd = argv[1];

  for (int i = 0; i < G_N_ELEMENTS (profile_commands); i++)
    {
      if (g_strcmp0 (cmd, profile_commands[i].name) == 0)
        {
          argc -= 1;
          argv += 1;

          if (!profile_commands[i].parse_args (argc, argv))
            return EXIT_FAILURE;

          return profile_commands[i].main ();
        }
    }

  g_printerr ("Usage: eos-profile <COMMAND> [OPTIONS...]\n");

  return EXIT_FAILURE;
}
