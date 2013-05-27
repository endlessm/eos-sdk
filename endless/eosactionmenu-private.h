/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_ACTION_MENU_H
#define EOS_ACTION_MENU_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_ACTION_MENU eos_action_menu_get_type()

#define EOS_ACTION_MENU(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                 EOS_TYPE_ACTION_MENU, EosActionMenu))

#define EOS_ACTION_MENU_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), \
                              EOS_TYPE_ACTION_MENU, EosActionMenuClass))

#define EOS_IS_ACTION_MENU(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                 EOS_TYPE_ACTION_MENU))

#define EOS_IS_ACTION_MENU_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                              EOS_TYPE_ACTION_MENU))

#define EOS_ACTION_MENU_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                EOS_TYPE_ACTION_MENU, EosActionMenuClass))

typedef struct _EosActionMenu EosActionMenu;
typedef struct _EosActionMenuClass EosActionMenuClass;
typedef struct _EosActionMenuPrivate EosActionMenuPrivate;

struct _EosActionMenu
{
  GtkGrid parent;

  EosActionMenuPrivate *priv;
};

struct _EosActionMenuClass
{
  GtkGridClass parent_class;
};

EOS_SDK_ALL_API_VERSIONS
GType        eos_action_menu_get_type              (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_action_menu_new                   ();

EOS_SDK_ALL_API_VERSIONS
GtkAction   *eos_action_menu_add_action            (EosActionMenu *menu,
                                                    const gchar   *first_property_name,
                                                    ...);

EOS_SDK_ALL_API_VERSIONS
GtkAction   *eos_action_menu_get_action            (EosActionMenu *menu,
                                                    const gchar   *name);

EOS_SDK_ALL_API_VERSIONS
GList       *eos_action_list_actions               (EosActionMenu *menu);


EOS_SDK_ALL_API_VERSIONS
void         eos_action_menu_remove_action         (EosActionMenu *menu,
                                                    GtkAction *action);

EOS_SDK_ALL_API_VERSIONS
void         eos_action_menu_remove_action_by_name (EosActionMenu *menu,
                                                    const gchar   *name);

G_END_DECLS

#endif /* EOS_ACTION_MENU_H */
