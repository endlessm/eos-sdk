/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_ACTION_BUTTON_H
#define EOS_ACTION_BUTTON_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_ACTION_BUTTON eos_action_button_get_type()

#define EOS_ACTION_BUTTON(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_ACTION_BUTTON, EosActionButton))

#define EOS_ACTION_BUTTON_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_ACTION_BUTTON, EosActionButtonClass))

#define EOS_IS_ACTION_BUTTON(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_ACTION_BUTTON))

#define EOS_IS_ACTION_BUTTON_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_ACTION_BUTTON))

#define EOS_ACTION_BUTTON_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_ACTION_BUTTON, EosActionButtonClass))

typedef struct _EosActionButton EosActionButton;
typedef struct _EosActionButtonClass EosActionButtonClass;
typedef struct _EosActionButtonPrivate EosActionButtonPrivate;

struct _EosActionButton
{
  GtkButton parent;

  EosActionButtonPrivate *priv;
};

struct _EosActionButtonClass
{
  GtkButtonClass parent_class;
};

GType               eos_action_button_get_type    (void) G_GNUC_CONST;

GtkWidget          *eos_action_button_new         (EosActionButtonSize size,
                                                   const gchar        *label,
                                                   const gchar        *icon_id);

void                eos_action_button_set_size    (EosActionButton    *button,
                                                   EosActionButtonSize size);

EosActionButtonSize eos_action_button_get_size    (EosActionButton    *button);

void                eos_action_button_set_label   (EosActionButton    *button,
                                                   const gchar        *label);

const gchar        *eos_action_button_get_label   (EosActionButton    *button);

void                eos_action_button_set_icon_id (EosActionButton    *button,
                                                   const gchar        *stock_id);

const gchar        *eos_action_button_get_icon_id (EosActionButton    *button);

G_END_DECLS

#endif /* EOS_ACTION_BUTTON_H */
