/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_ENUMS_H
#define EOS_ENUMS_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

/* Shared typedefs for enumerations */

/**
 * SECTION:eosenum
 * @Short_description: Public enumerated types used throughout the Endless SDK
 * @Title: Standard Enumerations
 */

/**
 * EosActionButtonSize:
 *
 * Built-in sizes for @EosActionButton
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
