/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_ENUMS_H
#define EOS_ENUMS_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

/* Shared typedefs for enumerations */

/**
 * SECTION:enums
 * @Short_description: Public enumerated types used throughout the Endless SDK
 * @Title: Standard Enumerations
 *
 * Public enumerated types used throughout the Endless SDK.
 */

/**
 * EosActionButtonSize:
 * @EOS_ACTION_BUTTON_SIZE_PRIMARY: size for primary buttons
 * @EOS_ACTION_BUTTON_SIZE_SECONDARY: size for secondary buttons
 * @EOS_ACTION_BUTTON_SIZE_TERTIARY: size for tertiary buttons
 * @EOS_ACTION_BUTTON_SIZE_QUATERNARY: size for quaternary buttons
 * @EOS_ACTION_BUTTON_SIZE_NUM_SIZES: total number of sizes
 *
 * Built-in sizes for internal action buttons.
 */
typedef enum
{
  EOS_ACTION_BUTTON_SIZE_PRIMARY = 0,
  EOS_ACTION_BUTTON_SIZE_SECONDARY,
  EOS_ACTION_BUTTON_SIZE_TERTIARY,
  EOS_ACTION_BUTTON_SIZE_QUATERNARY,
  EOS_ACTION_BUTTON_SIZE_NUM_SIZES
} EosActionButtonSize;


#endif /* EOS_ENUMS_H */
