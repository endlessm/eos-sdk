/* Copyright 2015 Endless Mobile, Inc. */

#ifndef EOS_LICENSE_H
#define EOS_LICENSE_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

G_BEGIN_DECLS

#define EOS_LICENSE_PUBLIC_DOMAN "Public Domain"

#define EOS_LICENSE_OWNER_PERMISSION "Owner Permission"

#define EOS_LICENSE_NO_LICENSE "No License"

EOS_SDK_AVAILABLE_IN_0_4
GFile *
eos_get_license_file (const gchar *license);

G_END_DECLS

#endif /* EOS_LICENSE_H */
