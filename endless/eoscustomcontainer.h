/* Copyright 2014 Endless Mobile, Inc. */

#ifndef EOS_CUSTOM_CONTAINER_H
#define EOS_CUSTOM_CONTAINER_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_CUSTOM_CONTAINER eos_custom_container_get_type()

#define EOS_CUSTOM_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_CUSTOM_CONTAINER, EosCustomContainer))

#define EOS_CUSTOM_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_CUSTOM_CONTAINER, EosCustomContainerClass))

#define EOS_IS_CUSTOM_CONTAINER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_CUSTOM_CONTAINER))

#define EOS_IS_CUSTOM_CONTAINER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_CUSTOM_CONTAINER))

#define EOS_CUSTOM_CONTAINER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_CUSTOM_CONTAINER, EosCustomContainerClass))

typedef struct _EosCustomContainer EosCustomContainer;
typedef struct _EosCustomContainerClass EosCustomContainerClass;

/**
 * EosCustomContainer:
 *
 * This structure contains no public members.
 */
struct _EosCustomContainer
{
  GtkContainer parent;
};

struct _EosCustomContainerClass
{
  GtkContainerClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_AVAILABLE_IN_0_0
GType        eos_custom_container_get_type                            (void) G_GNUC_CONST;

EOS_SDK_AVAILABLE_IN_0_0
GtkWidget   *eos_custom_container_new                                 (void);

G_END_DECLS

#endif /* EOS_CUSTOM_CONTAINER_H */
