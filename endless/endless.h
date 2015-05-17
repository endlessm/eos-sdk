/* Copyright 2013 Endless Mobile, Inc. */

#ifndef ENDLESS_H
#define ENDLESS_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define _EOS_SDK_INSIDE_ENDLESS_H

/* Pull in other header files */
#include "eostypes.h"
#include "eosapplication.h"
#include "eosflexygrid.h"
#include "eoslicense.h"
#include "eospagemanager.h"
#include "eoswindow.h"
#include "eoscustomcontainer.h"

#undef _EOS_SDK_INSIDE_ENDLESS_H

EOS_SDK_DEPRECATED_IN_0_0
gboolean eos_hello_sample_function (GFile   *file,
                                    GError **error);

EOS_SDK_AVAILABLE_IN_0_0
const gchar *   eos_get_system_personality      (void);

G_END_DECLS

#endif
