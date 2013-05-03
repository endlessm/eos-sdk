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
#include "eospagemanager.h"
#include "eoswindow.h"

#undef _EOS_SDK_INSIDE_ENDLESS_H

EOS_SDK_ALL_API_VERSIONS
gboolean eos_hello_sample_function (GFile   *file,
                                    GError **error);

G_END_DECLS

#endif

