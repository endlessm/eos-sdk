#include <glib.h>

extern void add_hello_tests (void);

int
main (int    argc,
      char **argv)
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  add_hello_tests ();

  return g_test_run ();
}
