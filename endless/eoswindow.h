/* Copyright 2013 Endless Mobile, Inc. */

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#ifndef EOS_WINDOW_H
#define EOS_WINDOW_H

#include "eostypes.h"

#include "eosapplication.h"

G_BEGIN_DECLS

#define EOS_TYPE_WINDOW eos_window_get_type()

#define EOS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_WINDOW, EosWindow))

#define EOS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_WINDOW, EosWindowClass))

#define EOS_IS_WINDOW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_WINDOW))

#define EOS_IS_WINDOW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_WINDOW))

#define EOS_WINDOW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_WINDOW, EosWindowClass))

typedef struct _EosWindow EosWindow;
typedef struct _EosWindowClass EosWindowClass;
typedef struct _EosWindowPrivate EosWindowPrivate;

/**
 * EosWindow:
 *
 * This class structure contains no public members.
 */
struct _EosWindow
{
  /*< private >*/
  GtkApplicationWindow parent;

  EosWindowPrivate *priv;
};

struct _EosWindowClass
{
  GtkApplicationWindowClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_ALL_API_VERSIONS
GType      eos_window_get_type (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget *eos_window_new      (EosApplication *application);

G_END_DECLS

#endif /* EOS_WINDOW_H */
