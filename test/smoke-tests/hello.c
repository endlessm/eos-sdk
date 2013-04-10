#include <glib-object.h>
#include <endless/endless.h>

int
main (int    argc,
      char **argv)
{
  g_type_init ();
  eos_hello_sample_function (NULL, NULL);
  return 0;
}
