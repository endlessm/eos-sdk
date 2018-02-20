#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>

#include "eos-profile-cmds.h"

static const EosProfileCmd profile_commands[] = {
  {
    .name = "help",
    .description = "Prints this help string",
    .usage = NULL,
    .parse_args = eos_profile_cmd_help_parse_args,
    .main = eos_profile_cmd_help_main,
  },
  {
    .name = "show",
    .description = "Prints a report from a capture file",
    .usage = "show <FILE> [FILE…]",
    .parse_args = eos_profile_cmd_show_parse_args,
    .main = eos_profile_cmd_show_main,
  },
  {
    .name = "convert",
    .description = "Converts a capture file to another format",
    .usage = "convert [OPTIONS…] <FILE>",
    .parse_args = eos_profile_cmd_convert_parse_args,
    .main = eos_profile_cmd_convert_main,
  },
  {
    .name = "diff",
    .description = "Compares FILES",
    .usage = "diff [OPTIONS…] <FILES>",
    .parse_args = eos_profile_cmd_diff_parse_args,
    .main = eos_profile_cmd_diff_main,
  },
};

void
eos_profile_foreach_cmd (EosProfileCmdCallback cb,
                         gpointer              data)
{
  for (int i = 0; i < G_N_ELEMENTS (profile_commands); i++)
    {
      if (cb (&profile_commands[i], data))
        break;
    }
}

int
main (int argc,
      char *argv[])
{
  setlocale (LC_ALL, "");
  textdomain (GETTEXT_PACKAGE);

  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

  const char *cmd = NULL;

  if (argc < 2 ||
      g_strcmp0 (argv[1], "-h") == 0 ||
      g_strcmp0 (argv[1], "-?") == 0 ||
      g_strcmp0 (argv[1], "--help") == 0)
    {
      cmd = "help";
    }
  else
    cmd = argv[1];

  for (int i = 0; i < G_N_ELEMENTS (profile_commands); i++)
    {
      if (g_strcmp0 (cmd, profile_commands[i].name) == 0)
        {
          argc -= 1;
          argv += 1;

          if (!profile_commands[i].parse_args (argc, argv))
            {
              const char *usage = profile_commands[i].usage;

              if (usage == NULL)
                usage = profile_commands[i].name;

              g_printerr ("Usage: eos-profile %s\n", usage);

              return EXIT_FAILURE;
            }

          return profile_commands[i].main ();
        }
    }

  g_printerr ("Usage: eos-profile <COMMAND> [OPTIONS...]\n");

  return EXIT_FAILURE;
}
