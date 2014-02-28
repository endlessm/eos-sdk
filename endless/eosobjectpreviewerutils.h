/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_OBJECT_PREVIEWER_UTILS_H
#define EOS_OBJECT_PREVIEWER_UTILS_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

/* Utils functions that need C API access for the object previewer. */
EOS_SDK_ALL_API_VERSIONS
GList *eos_get_evince_supported_formats (void);

#endif /* EOS_OBJECT_PREVIEWER_UTILS_H */
