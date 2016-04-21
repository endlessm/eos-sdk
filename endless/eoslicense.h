/* Copyright 2015-2016 Endless Mobile, Inc. */

#ifndef EOS_LICENSE_H
#define EOS_LICENSE_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

G_BEGIN_DECLS

/**
 * EOS_LICENSE_PUBLIC_DOMAN:
 *
 * A string constant that represents the "Public domain" attribution level.
 *
 * Since: 0.4
 */
#define EOS_LICENSE_PUBLIC_DOMAN "Public domain"

/**
 * EOS_LICENSE_OWNER_PERMISSION:
 *
 * A string constant that represents the "Owner permission" attribution level.
 *
 * Since: 0.4
 */
#define EOS_LICENSE_OWNER_PERMISSION "Owner permission"

/**
 * EOS_LICENSE_NO_LICENSE:
 *
 * A string constant that represents the "No license" attribution level.
 *
 * Since: 0.4
 */
#define EOS_LICENSE_NO_LICENSE "No license"

EOS_SDK_AVAILABLE_IN_0_4
GFile *
eos_get_license_file (const gchar *license);

EOS_SDK_AVAILABLE_IN_0_4
const char *
eos_get_license_display_name (const gchar *license);


G_END_DECLS

#endif /* EOS_LICENSE_H */
