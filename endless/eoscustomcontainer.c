/* Copyright 2014-2016 Endless Mobile, Inc. */

#include "config.h"
#include "eoscustomcontainer.h"

#include <gtk/gtk.h>

/**
 * SECTION:custom-container
 * @short_description: For gjs container implementations
 * @title: Custom Container
 *
 * This container allows for implementing a custom size allocate routine in
 * gjs. This container implements the bare minimum of virtual functions from
 * GtkContainer, add, remove and forall. Add and remove simply append to and
 * remove from an internal list, and forall iterates over that list. Forall
 * cannot be implemented in gjs, it's not supported by gobject-introspection,
 * so this is needed for custom gjs containers. This class will not
 * size_allocate any children or ever queue_resize, so that is up to
 * subclasses in gjs.
 *
 * Here's an example gjs program which allocates a GtkFrame the top right
 * quarter of it's allocation.
 * |[
 * const TestContainer = Lang.Class({
 *   Name: 'TestContainer',
 *   Extends: Endless.CustomContainer,
 *
 *   _init: function() {
 *     this.parent();
 *
 *     this._frame = new Gtk.Frame();
 *     this.add(this._frame);
 *   },
 *
 *   vfunc_size_allocate: function (alloc) {
 *     this.parent(alloc);
 *     alloc.width = alloc.width / 2;
 *     alloc.height = alloc.height / 2;
 *     this._frame.size_allocate(alloc);
 *   }
 * });
 * ]|
 */

typedef struct {
  GList *children;
} EosCustomContainerPrivate;

G_DEFINE_TYPE_WITH_PRIVATE (EosCustomContainer, eos_custom_container, GTK_TYPE_CONTAINER)

static void
eos_custom_container_add (GtkContainer *container,
                          GtkWidget    *child)
{
  EosCustomContainer *self = EOS_CUSTOM_CONTAINER (container);
  EosCustomContainerPrivate *priv = eos_custom_container_get_instance_private (self);

  priv->children = g_list_prepend (priv->children, child);
  gtk_widget_set_parent (child, GTK_WIDGET (container));
}

static void
eos_custom_container_remove (GtkContainer *container,
                             GtkWidget    *child)
{
  EosCustomContainer *self = EOS_CUSTOM_CONTAINER (container);
  EosCustomContainerPrivate *priv = eos_custom_container_get_instance_private (self);

  priv->children = g_list_remove (priv->children, child);
  gtk_widget_unparent (child);
}

static void
eos_custom_container_forall (GtkContainer *container,
                             gboolean      include_internals,
                             GtkCallback   callback,
                             gpointer      callback_data)
{
  EosCustomContainer *self = EOS_CUSTOM_CONTAINER (container);
  EosCustomContainerPrivate *priv = eos_custom_container_get_instance_private (self);

  g_list_foreach (priv->children, (GFunc)callback, callback_data);
}

static void
eos_custom_container_class_init (EosCustomContainerClass *klass)
{
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  container_class->add = eos_custom_container_add;
  container_class->remove = eos_custom_container_remove;
  container_class->forall = eos_custom_container_forall;
}

static void
eos_custom_container_init (EosCustomContainer *self)
{
  GtkWidget *widget = GTK_WIDGET (self);
  gtk_widget_set_has_window (widget, FALSE);
}

/**
 * eos_custom_container_new:
 *
 * Creates a new custom container.
 *
 * Returns: the custom container.
 */
GtkWidget *
eos_custom_container_new (void)
{
  return g_object_new (EOS_TYPE_CUSTOM_CONTAINER, NULL);
}
