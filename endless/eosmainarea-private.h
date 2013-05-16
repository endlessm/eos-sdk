/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_MAIN_AREA_H
#define EOS_MAIN_AREA_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

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

struct _EosMainArea
{
  GtkContainer parent;

  EosMainAreaPrivate *priv;
};

struct _EosMainAreaClass
{
  GtkContainerClass parent_class;
};

GType      eos_main_area_get_type    (void) G_GNUC_CONST;

GtkWidget *eos_main_area_new         (void);

void       eos_main_area_set_toolbox (EosMainArea *self,
                                      GtkWidget   *toolbox);
GtkWidget *eos_main_area_get_toolbox (EosMainArea *self);
void       eos_main_area_set_content (EosMainArea *self,
                                      GtkWidget   *content);
GtkWidget *eos_main_area_get_content (EosMainArea *self);
void       eos_main_area_set_actions (EosMainArea *self,
                                      gboolean     actions_visible);
gboolean   eos_main_area_get_actions (EosMainArea *self);

G_END_DECLS

#endif /* EOS_MAIN_AREA_H */
