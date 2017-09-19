/* Copyright 2017 Endless Mobile, Inc. */

#pragma once

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"
#include <glib.h>

G_BEGIN_DECLS

/**
 * EosProfileProbe:
 *
 * An opaque identifier for a profiling probe.
 *
 * Since: 0.6
 */
typedef struct _EosProfileProbe         EosProfileProbe;

/**
 * EOS_PROFILE_PROBE:
 * @name: the name of the profiling probe
 *
 * A convenience macro that creates a profiling probe at the given
 * location.
 *
 * Since: 0.6
 */
#define EOS_PROFILE_PROBE(name) \
  eos_profile_probe_start (__FILE__, __LINE__, G_STRFUNC, name)

EOS_SDK_AVAILABLE_IN_0_6
GType eos_profile_probe_get_type (void) G_GNUC_CONST;

EOS_SDK_AVAILABLE_IN_0_6
EosProfileProbe *       eos_profile_probe_start (const char      *file,
                                                 gsize            line,
                                                 const char      *function,
                                                 const char      *name);
EOS_SDK_AVAILABLE_IN_0_6
void                    eos_profile_probe_stop  (EosProfileProbe *probe);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(EosProfileProbe, eos_profile_probe_stop)

G_END_DECLS
