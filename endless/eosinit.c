/* Copyright 2013 Endless Mobile, Inc. */

#include <config.h>
#include <glib.h>
#include <glib/gi18n-lib.h>

#include "eosinit-private.h"

/* Constructors supported since GCC 2.7; I have this on GLib's authority. This
should also work on Clang. */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)

#define _EOS_CONSTRUCTOR(func) static void __attribute__((constructor)) func (void);
#define _EOS_DESTRUCTOR(func) static void __atrribute__((destructor)) func (void);

#else

#error "We do not currently support constructors for your compiler."

#endif /* compiler version */

static gboolean _eos_initialized = FALSE;

/*
 * _eos_init:
 *
 * This function initializes the library. It is called automatically when the
 * library is loaded.
 */
_EOS_CONSTRUCTOR(_eos_init);
static void
_eos_init (void)
{
  if (G_UNLIKELY (!_eos_initialized))
    {
      /* Initialize Gettext */
      bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");

      _eos_initialized = TRUE;
    }
}

/*
 * eos_is_inited:
 *
 * For testing purposes.
 */
gboolean
eos_is_inited (void)
{
  return _eos_initialized;
}

/**
 * eos_get_system_personality:
 *
 * Retrieves the "personality" of the system.
 *
 * The personality is a unique string that identifies the installation
 * of EndlessOS for a specific country or audience. The availability of
 * certain applications, or their content, is determined by this value.
 *
 * Return value: (transfer none): a string, owned by the Endless SDK,
 *   with the name of the personality. You should never free or modify
 *   the returned string.
 */
const gchar *
eos_get_system_personality (void)
{
  static gchar *personality;

  if (g_once_init_enter (&personality))
    {
      gchar *tmp;

      tmp = g_strdup (g_getenv ("ENDLESS_OS_PERSONALITY"));
      if (tmp == '\0')
        {
          g_free (tmp);
          tmp = NULL;
        }

      if (tmp == NULL)
        {
          char *path = g_build_filename (DATADIR,
                                         "EndlessOS",
                                         "personality.txt",
                                         NULL);

          GError *error = NULL;
          g_file_get_contents (path, &tmp, NULL, &error);
          if (error != NULL)
            {
              g_critical ("No personality defined: %s", error->message);
              g_error_free (error);
              tmp = NULL;
            }
        }

      if (tmp == NULL)
        tmp = g_strdup ("Default");

      g_once_init_leave (&personality, tmp);
    }

  return personality;
}
