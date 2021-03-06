/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_MACROS_H
#define EOS_MACROS_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

/* Shared preprocessor macros */

#define EOS_ENUM_VALUE(value, nick)     { value, #value, #nick },

#define EOS_DEFINE_ENUM_TYPE(EnumType, enum_type, values) \
GType \
enum_type##_get_type (void) \
{ \
  static volatile gsize g_define_type_id__volatile = 0; \
  if (g_once_init_enter (&g_define_type_id__volatile)) \
    { \
      static const GEnumValue v[] = { \
        values \
        { 0, NULL, NULL }, \
      }; \
      GType g_define_type_id = \
        g_enum_register_static (g_intern_static_string (#EnumType), v); \
\
      g_once_init_leave (&g_define_type_id__volatile, g_define_type_id); \
    } \
  return g_define_type_id__volatile; \
}

/**
 * EOS_STYLE_CLASS_COMPOSITE:
 *
 * A CSS class to match a window on a composite TV.
 *
 * This is added to #EosWindow when it is detected to be on a composite TV
 * screen.
 *
 * Since: 0.6
 */
#define EOS_STYLE_CLASS_COMPOSITE "composite"

#endif /* EOS_MACROS_H */
