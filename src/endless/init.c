#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

/*
 * _eos_init:
 *
 * This function initializes the library. It must be called in every entry
 * point of the library.
 */
void
_eos_init (void)
{
  static size_t initialized = 0;

  /* Only do the initialization once */
  if (g_once_init_enter (&initialized))
    {
      /* Initialize Gettext */
      bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

      g_once_init_leave (&initialized, 1);
    }

  g_assert (initialized);
}
