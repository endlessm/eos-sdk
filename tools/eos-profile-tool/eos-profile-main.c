#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glib.h>

#include "eos-profile-cmds.h"

static gboolean opt_cmd = FALSE;

int
main (int argc,
      char *argv[])
{
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
