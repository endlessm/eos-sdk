/* Copyright 2013 Endless Mobile, Inc. */

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#ifndef EOS_MAIN_AREA_H
#define EOS_MAIN_AREA_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_MAIN_AREA eos_main_area_get_type()

#define EOS_MAIN_AREA(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_MAIN_AREA, EosMainArea))

#define EOS_MAIN_AREA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_MAIN_AREA, EosMainAreaClass))

#define EOS_IS_MAIN_AREA(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_MAIN_AREA))

#define EOS_IS_MAIN_AREA_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_MAIN_AREA))

#define EOS_MAIN_AREA_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_MAIN_AREA, EosMainAreaClass))

typedef struct _EosMainArea EosMainArea;
typedef struct _EosMainAreaClass EosMainAreaClass;
typedef struct _EosMainAreaPrivate EosMainAreaPrivate;

/**
 * EosMainArea:
 *
 * This class structure contains no public members.
 */
struct _EosMainArea
{
  /*< private >*/
  GtkContainer parent;

  EosMainAreaPrivate *priv;
};

struct _EosMainAreaClass
{
  GtkContainerClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_ALL_API_VERSIONS
GType      eos_main_area_get_type (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget *eos_main_area_new();

void *eos_main_area_set_toolbar (EosMainArea *main_area, GtkWidget *toolbar);

GtkWidget *eos_main_area_get_toolbar (EosMainArea *main_area);

void *eos_main_area_set_content (EosMainArea *main_area, GtkWidget *content);

GtkWidget *eos_main_area_get_content (EosMainArea *main_area);

void *eos_main_area_set_actions (EosMainArea *main_area, gboolean actions);

gboolean eos_main_area_get_actions (EosMainArea *main_area);

G_END_DECLS

#endif /* EOS_MAIN_AREA_H */
