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
