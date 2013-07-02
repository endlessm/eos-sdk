/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosactionmenu-private.h"

#include "eosactionbutton.h"
#include <glib-object.h>
#include <gtk/gtk.h>
#include <math.h>

#define _EOS_STYLE_CLASS_ACTION_MENU "action-menu"

/*
 * SECTION:action-menu
 * @short_description: Adding actions to the page
 * @title: Action Menu
 */


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
eos_action_menu_init (EosActionMenu *self)
{
  EosActionMenuPrivate *priv;
  GtkStyleContext *context;

  self->priv = EOS_ACTION_MENU_PRIVATE (self);
  priv = self->priv;

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_ACTION_MENU);

  // TODO : name?
  priv->action_group = gtk_action_group_new ("EosActionMenu");

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);
  gtk_widget_set_vexpand (GTK_WIDGET (self), TRUE);
  gtk_widget_set_halign (GTK_WIDGET (self), GTK_ALIGN_CENTER);
  gtk_widget_set_valign (GTK_WIDGET (self), GTK_ALIGN_CENTER);
}

/* ******* LIFECYCLE ******* */

/*
 * eos_action_menu_new:
 *
 * Returns: a new instance
 */
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

/*
 * eos_action_menu_add_action:
 * @menu: a #EosActionMenu
 * @action: a #GtkAction: name, label, icon-name, is-important.
 *
 * Adds an action to the #EosActionMenu, using its name, label, icon-name and
 * is-important properties.
 */
void
eos_action_menu_add_action (EosActionMenu *menu,
                            GtkAction *action)
{
  EosActionMenuPrivate *priv;

  g_return_if_fail (EOS_IS_ACTION_MENU (menu));
  priv = menu->priv;

  if (action)
    {
      gtk_action_group_add_action (priv->action_group, action);

      EosActionButtonSize size = gtk_action_get_is_important (action) ?
                                             EOS_ACTION_BUTTON_SIZE_PRIMARY :
                                             EOS_ACTION_BUTTON_SIZE_SECONDARY;

      GtkWidget *action_button = eos_action_button_new (size,
                                                        gtk_action_get_label (action),
                                                        gtk_action_get_icon_name (action));

      gtk_activatable_set_related_action (GTK_ACTIVATABLE (action_button), action);

      // TODO : maybe we need a finer control, taking is-important into account?
      gtk_grid_attach_next_to (GTK_GRID (menu), action_button, NULL,
                               GTK_POS_BOTTOM, 1, 1);
    }
}

/*
 * eos_action_menu_get_action:
 * @menu: an #EosActionMenu
 * @name: the name of the action to retrieve
 *
 * Retrieves an action.
 *
 * Returns: (transfer none): the #GtkAction
 */
GtkAction *
eos_action_menu_get_action (EosActionMenu *menu,
                            const gchar *name)
{
  EosActionMenuPrivate *priv;
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  priv = menu->priv;

  return gtk_action_group_get_action (priv->action_group, name);
}

/*
 * eos_action_menu_list_actions:
 * @menu: an #EosActionMenu
 *
 * Returns: (element-type GList) (transfer container): an allocated list of the action objects in the action group
 */
GList *
eos_action_menu_list_actions (EosActionMenu *menu)
{
  EosActionMenuPrivate *priv;
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  priv = menu->priv;

  return gtk_action_group_list_actions (priv->action_group);
}

/*
 * eos_action_menu_remove_action:
 * @menu: an #EosActionMenu
 * @action: the action to remove
 *
 * Removes an action
 */
void
eos_action_menu_remove_action (EosActionMenu *menu,
                               GtkAction *action)
{
  EosActionMenuPrivate *priv;
  g_return_if_fail (EOS_IS_ACTION_MENU (menu));
  priv = menu->priv;

  gtk_action_group_remove_action(priv->action_group, action);
}

/*
 * eos_action_menu_remove_action_by_name:
 * @menu: an #EosActionMenu
 * @name: the name of the action to remove
 *
 * Removes the action with the given name
 */
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

