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


typedef struct {
  GtkWidget *overlay;
  GtkWidget *center_grid;
  GtkWidget *bottom_grid;

  GtkActionGroup *action_group;
} EosActionMenuPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosActionMenu, eos_action_menu, GTK_TYPE_FRAME)

static void
eos_action_menu_dispose (GObject *object);

static void
eos_action_menu_finalize (GObject *object);

/* ******* INIT ******* */

static void
eos_action_menu_class_init (EosActionMenuClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = eos_action_menu_dispose;
  object_class->finalize = eos_action_menu_finalize;
}

static void
eos_action_menu_init (EosActionMenu *self)
{
  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (self);
  GtkStyleContext *context;

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_ACTION_MENU);

  priv->overlay = gtk_overlay_new ();
  g_object_set (G_OBJECT (priv->overlay),
                "halign", GTK_ALIGN_FILL,
                "valign", GTK_ALIGN_FILL,
                "hexpand", TRUE,
                "vexpand", TRUE,
                NULL);

  priv->center_grid = gtk_grid_new ();
  g_object_set (G_OBJECT (priv->center_grid),
                "orientation", GTK_ORIENTATION_VERTICAL,
                "halign", GTK_ALIGN_CENTER,
                "valign", GTK_ALIGN_CENTER,
                NULL);

  priv->bottom_grid = gtk_grid_new ();
  g_object_set (G_OBJECT (priv->bottom_grid),
                "orientation", GTK_ORIENTATION_VERTICAL,
                "halign", GTK_ALIGN_CENTER,
                "valign", GTK_ALIGN_END,
                NULL);

  // this is ugly, but needed so the overlay takes all the available space
  GtkWidget* placeholder = gtk_event_box_new();
  gtk_widget_set_hexpand (placeholder, TRUE);
  gtk_widget_set_vexpand (placeholder, TRUE);
  gtk_container_add (GTK_CONTAINER (priv->overlay), placeholder);

  gtk_overlay_add_overlay (GTK_OVERLAY (priv->overlay), priv->center_grid);
  gtk_overlay_add_overlay (GTK_OVERLAY (priv->overlay), priv->bottom_grid);

  gtk_container_add (GTK_CONTAINER (self), priv->overlay);

  // TODO : name?
  priv->action_group = gtk_action_group_new ("EosActionMenu");

  gtk_widget_set_hexpand (GTK_WIDGET (self), TRUE);
  gtk_widget_set_vexpand (GTK_WIDGET (self), TRUE);
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
 * Adds an action to the #EosActionMenu, using its #GtkAction:name,
 * #GtkAction:label, #GtkAction:icon-name, #GtkAction:is-important and
 * #GtkAction:stock-id properties.
 *
 * Cancel, close and delete actions are placed at the bottom of the menu. To
 * indicate this, set the #GtkAction:stock-id property to one of
 * #GTK_STOCK_CANCEL, #GTK_STOCK_CLOSE or #GTK_STOCK_DELETE. All other values of
 * this property will be ignored.
 *
 */
void
eos_action_menu_add_action (EosActionMenu *menu,
                            GtkAction *action)
{
  g_return_if_fail (EOS_IS_ACTION_MENU (menu));

  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (menu);
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

      if (g_strcmp0 (gtk_action_get_stock_id (action), GTK_STOCK_CANCEL) == 0 ||
          g_strcmp0 (gtk_action_get_stock_id (action), GTK_STOCK_CLOSE) == 0  ||
          g_strcmp0 (gtk_action_get_stock_id (action), GTK_STOCK_DELETE) == 0)
        {
          gtk_container_add (GTK_CONTAINER (priv->bottom_grid), action_button);
        }
      else
        {
          gtk_container_add (GTK_CONTAINER (priv->center_grid), action_button);
        }
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
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (menu);

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
  g_return_val_if_fail (EOS_IS_ACTION_MENU (menu), NULL);
  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (menu);

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
  g_return_if_fail (EOS_IS_ACTION_MENU (menu));
  g_return_if_fail (GTK_IS_ACTION (action));

  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (menu);
  GList *children, *i;
  GtkWidget *target_child = NULL;

  gtk_action_group_remove_action(priv->action_group, action);

  children = gtk_container_get_children (GTK_CONTAINER (priv->center_grid));

  children = g_list_concat (children,
                            gtk_container_get_children (GTK_CONTAINER (priv->bottom_grid)));

  for (i = children; i != NULL; i = i->next)
    {
      GtkWidget *child = i->data;
      GtkAction *childs_action = gtk_activatable_get_related_action (GTK_ACTIVATABLE (child));

      if (childs_action != NULL &&
          g_strcmp0 (gtk_action_get_name (childs_action), gtk_action_get_name (action)) == 0)
        {
          target_child = child;
          break;
        }
    }

  if (target_child != NULL)
    {
      gtk_widget_destroy (target_child);
    }
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
  g_return_if_fail (EOS_IS_ACTION_MENU (menu));

  GtkAction *action;
  EosActionMenuPrivate *priv = eos_action_menu_get_instance_private (menu);

  action = gtk_action_group_get_action (priv->action_group, name);
  if (action)
    {
      eos_action_menu_remove_action (menu, action);
    }
}

/* ******* LAYOUT AND VISUALS ******* */

