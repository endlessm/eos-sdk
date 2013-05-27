/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosactionmenu-private.h"

#include "eosactionbutton-private.h"
#include <glib-object.h>
#include <gtk/gtk.h>
#include <math.h>

#define _EOS_STYLE_CLASS_ACTION_MENU "action-menu"

G_DEFINE_TYPE (EosActionMenu, eos_action_menu, GTK_TYPE_GRID)

#define EOS_ACTION_MENU_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_ACTION_MENU, EosActionMenuPrivate))

struct _EosActionMenuPrivate
{
  GtkActionGroup *action_group;
};

static void
eos_action_menu_dispose (GObject *object);

static void
eos_action_menu_finalize (GObject *object);

/* ******* INIT ******* */

static void
eos_action_menu_class_init (EosActionMenuClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosActionMenuPrivate));

  object_class->dispose = eos_action_menu_dispose;
  object_class->finalize = eos_action_menu_finalize;
}

static void
eos_action_menu_init (EosActionMenu *menu)
{
  EosActionMenuPrivate *priv;

  menu->priv = EOS_ACTION_MENU_PRIVATE (menu);
  priv = menu->priv;

  // TODO : name?
  priv->action_group = gtk_action_group_new ("EosActionMenu");
}

/* ******* LIFECYCLE ******* */

GtkWidget *
eos_action_menu_new ()
{
  return g_object_new (EOS_TYPE_ACTION_MENU, NULL);
}

static void
eos_action_menu_dispose (GObject *object)
{
  G_OBJECT_CLASS (eos_action_menu_parent_class)->dispose (object);
}

static void
eos_action_menu_finalize (GObject *object)
{
  G_OBJECT_CLASS (eos_action_menu_parent_class)->finalize (object);
}

/* ******* ACTION GROUP MGMT ******* */

GtkAction *
eos_action_menu_add_action (EosActionMenu *menu,
                            const gchar *first_property_name,
                            ...)
{
  EosActionMenuPrivate *priv;
  GtkAction *action;
  va_list var_args;

  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  menu->priv = EOS_ACTION_MENU_PRIVATE (menu);

  va_start (var_args, first_property_name);
  action = (GtkAction*) g_object_new_valist (GTK_TYPE_ACTION, first_property_name, var_args);
  va_end (var_args);

  gtk_action_group_add_action(priv->action_group, action);

  // TODO : create and wire up the action button
}

GtkAction *
eos_action_menu_get_action (EosActionMenu *menu,
                            const gchar *name)
{
  EosActionMenuPrivate *priv;
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  menu->priv = EOS_ACTION_MENU_PRIVATE (menu);

  return gtk_action_group_get_action (priv->action_group, name);
}

GList *
eos_action_list_actions (EosActionMenu *menu)
{
  EosActionMenuPrivate *priv;
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  priv = menu->priv;

  return gtk_action_group_list_actions (priv->action_group);
}

void
eos_action_menu_remove_action (EosActionMenu *menu,
                               GtkAction *action)
{
  EosActionMenuPrivate *priv;
  g_return_if_fail (EOS_IS_ACTION_MENU (menu));
  priv = menu->priv;

  gtk_action_group_remove_action(priv->action_group, action);
}

void
eos_action_menu_remove_action_by_name (EosActionMenu *menu,
                                       const gchar *name)
{
  GtkAction *action;
  EosActionMenuPrivate *priv;

  g_return_if_fail (EOS_IS_ACTION_MENU (menu));
  priv = menu->priv;

  action = gtk_action_group_get_action (priv->action_group, name);
  if (action)
    {
      gtk_action_group_remove_action (priv->action_group, action);
    }
}

/* ******* LAYOUT AND VISUALS ******* */

