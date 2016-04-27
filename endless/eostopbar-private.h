/* Copyright (C) 2013-2016 Endless Mobile, Inc. */

#ifndef EOS_TOP_BAR_H
#define EOS_TOP_BAR_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_TOP_BAR eos_top_bar_get_type()

#define EOS_TOP_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_TOP_BAR, EosTopBar))

#define EOS_TOP_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_TOP_BAR, EosTopBarClass))

#define EOS_IS_TOP_BAR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_TOP_BAR))

#define EOS_IS_TOP_BAR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_TOP_BAR))

#define EOS_TOP_BAR_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_TOP_BAR, EosTopBarClass))

typedef struct _EosTopBar EosTopBar;
typedef struct _EosTopBarClass EosTopBarClass;

struct _EosTopBar
{
  GtkEventBox parent;
};

struct _EosTopBarClass
{
  GtkEventBoxClass parent_class;
};

GType      eos_top_bar_get_type                (void) G_GNUC_CONST;

GtkWidget *eos_top_bar_new                     (void);

void       eos_top_bar_set_left_widget         (EosTopBar *self,
                                                GtkWidget *left_top_bar_widget);

void       eos_top_bar_set_center_widget       (EosTopBar *self,
                                                GtkWidget *center_top_bar_widget);

gboolean   eos_top_bar_get_show_credits_button (EosTopBar *self);

void       eos_top_bar_set_show_credits_button (EosTopBar *self,
                                                gboolean   show_credits_button);

G_END_DECLS

#endif /* EOS_TOP_BAR_H */
