/* Copyright 2017 Endless Mobile, Inc. */

#include <stdlib.h>
#include <endless/endless.h>

#include "run-tests.h"

static void
test_profile_stdout (void)
{
  g_autoptr(EosProfileProbe) probe = EOS_PROFILE_PROBE ("/sdk/profile/main");

  for (int i = 0; i < 256000; i++)
    {
      g_autoptr(EosProfileProbe) inner = EOS_PROFILE_PROBE ("/sdk/profile/inner-loop");

      GArray *array = g_array_new (FALSE, FALSE, sizeof (int));

      for (int j = 0; j < 1000; j++)
        g_array_append_val (array, j);

      g_array_unref (array);
    }
}

void
add_profile_tests (void)
{
  g_test_add_func ("/profile/stdout", test_profile_stdout);
}
