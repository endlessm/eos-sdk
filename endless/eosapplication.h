/* Copyright 2013 Endless Mobile, Inc. */

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#ifndef EOS_APPLICATION_H
#define EOS_APPLICATION_H

#include "eostypes.h"

#include <gtk/gtk.h>

#define EOS_TYPE_APPLICATION eos_application_get_type()

#define EOS_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_APPLICATION, EosApplication))

#define EOS_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_APPLICATION, EosApplicationClass))

#define EOS_IS_APPLICATION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_APPLICATION))

#define EOS_IS_APPLICATION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_APPLICATION))

#define EOS_APPLICATION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_APPLICATION, EosApplicationClass))

typedef struct _EosApplication EosApplication;
typedef struct _EosApplicationClass EosApplicationClass;
typedef struct _EosApplicationPrivate EosApplicationPrivate;

/**
 * EosApplication:
 *
 * This class structure contains no public members.
 */
struct _EosApplication
{
  /*< private >*/
  GtkApplication parent;

  EosApplicationPrivate *priv;
};

struct _EosApplicationClass
{
  GtkApplicationClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_ALL_API_VERSIONS
GType           eos_application_get_type (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
EosApplication *eos_application_new      (const gchar      *application_id,
                                          GApplicationFlags flags);

G_END_DECLS

#endif /* EOS_APPLICATION_H */
