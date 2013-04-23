/* Copyright 2013 Endless Mobile, Inc. */

#include "eosmainarea.h"

#include <gtk/gtk.h>

/**
 * SECTION:main_area
 * @short_description: The main area for your application, under the top toolbar.
 * @title: EosMainArea
 *
 * Stub
 */

G_DEFINE_TYPE (EosMainArea, eos_main_area, GTK_TYPE_CONTAINER)

#define MAIN_AREA_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_MAIN_AREA, EosMainAreaPrivate))

struct _EosMainAreaPrivate
{
  GtkWidget *toolbar;
  GtkWidget *content;
  guint actions : 1;
};

enum
{
  PROP_0,
  PROP_TOOLBAR,
  PROP_CONTENT,
  PROP_ACTIONS,
  NPROPS
};

static GParamSpec *eos_main_area_props[NPROPS] = { NULL, };

static void
eos_main_area_set_property (GObject      *object,
                            guint        property_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  EosMainArea *self = EOS_MAIN_AREA (object);

  switch (property_id)
    {
    case PROP_TOOLBAR:
      eos_main_area_set_toolbar (self, g_value_get_object (value));
      break;

    case PROP_CONTENT:
      eos_main_area_set_content (self, g_value_get_object (value));
      break;

    case PROP_ACTIONS:
      eos_main_area_set_actions (self, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_main_area_get_property (GObject    *object,
                            guint       property_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  EosMainArea *self = EOS_MAIN_AREA (object);

  switch (property_id)
    {
    case PROP_TOOLBAR:
      g_value_set_object (value, eos_main_area_get_toolbar (self));
      break;

    case PROP_CONTENT:
      g_value_set_object (value, eos_main_area_get_content (self));
      break;

    case PROP_ACTIONS:
      g_value_set_boolean (value, eos_main_area_get_actions (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_main_area_add(GtkContainer *container, GtkWidget *widget){
  EosMainArea *self = EOS_MAIN_AREA (container);

  /* Print out warning message?? */

  eos_main_area_set_content(self, widget);
}

static void
eos_main_area_remove(GtkContainer *container, GtkWidget *widget){
  EosMainArea *self = EOS_MAIN_AREA(container);
  EosMainAreaPrivate *priv = self->priv;
  if(priv->content == widget)
    eos_main_area_set_content(self, NULL);
  else if(priv->toolbar == widget)
    eos_main_area_set_toolbar(self, NULL);

  /* TODO: remove actions */

}

static void
eos_main_area_get_preferred_width(GtkWidget *widget, gint *minimal, gint *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *content = self->priv->content;

  if (content && gtk_widget_get_visible (content)) {
    gtk_widget_get_preferred_width(content, minimal, natural);
  }
}

static void
eos_main_area_get_preferred_height(GtkWidget *widget, gint *minimal, gint *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *content = self->priv->content;

  if (content && gtk_widget_get_visible (content)) {
    gtk_widget_get_preferred_height(content, minimal, natural);
  }
}

static void
eos_main_area_get_preferred_width_for_height(GtkWidget *widget,
                                             gint for_height,
                                             gint *minimal,
                                             gint *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *content = self->priv->content;

  if (content && gtk_widget_get_visible (content)) {
    gtk_widget_get_preferred_width_for_height(content, for_height, minimal, natural);
  }
}

static void
eos_main_area_get_preferred_height_for_width(GtkWidget *widget,
                                             gint for_width,
                                             gint *minimal,
                                             gint *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *content = self->priv->content;

  if (content && gtk_widget_get_visible (content)) {
    gtk_widget_get_preferred_width_for_height(content, for_width, minimal, natural);
  }
}

static void
eos_main_size_allocate(GtkWidget *widget, GtkAllocation *allocation)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *content = self->priv->content;

  gtk_widget_set_allocation (widget, allocation);
  if (content && gtk_widget_get_visible (content)) {
    gtk_widget_size_allocate(content, allocation);
  }
}

void
eos_main_area_forall(GtkContainer *container,
                     gboolean      include_internals,
                     GtkCallback   callback,
                     gpointer      callback_data) {
  EosMainArea *self = EOS_MAIN_AREA (container);
  EosMainAreaPrivate *priv = self->priv;

  if (priv->toolbar)
    (* callback) (priv->toolbar, callback_data);

  if (priv->content)
    (* callback) (priv->content, callback_data);
}

static void
eos_main_area_class_init (EosMainAreaClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosMainAreaPrivate));

  object_class->set_property = eos_main_area_set_property;
  object_class->get_property = eos_main_area_get_property;

  widget_class->get_preferred_width = eos_main_area_get_preferred_width;
  widget_class->get_preferred_height = eos_main_area_get_preferred_height;
  widget_class->get_preferred_width_for_height = eos_main_area_get_preferred_width_for_height;
  widget_class->get_preferred_height_for_width = eos_main_area_get_preferred_height_for_width;
  widget_class->size_allocate = eos_main_size_allocate;

  container_class->forall = eos_main_area_forall;
  container_class->add = eos_main_area_add;
  container_class->remove = eos_main_area_remove;

  /**
   * EosMainArea:toolbar:
   *
   * A widget to be added to the toolbar area on the left side of the screen.
   * Sized to blah.... If not set, this area will not be allocated by the
   * container.
   */
  eos_main_area_props[PROP_TOOLBAR] =
    g_param_spec_object ("toolbar", "Toolbar",
                         "Toolbar widget to be displayed on left side of "
                         "window",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READABLE | G_PARAM_WRITABLE);

  /**
   * EosMainArea:content:
   *
   * A widget with the main application content to be added in the center.
   * Should always be set.
   */
  eos_main_area_props[PROP_CONTENT] =
    g_param_spec_object ("content", "Content",
                         "Content widget to be displayed in center of window",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READABLE | G_PARAM_WRITABLE);
  /**
   * EosMainArea:actions:
   *
   * Stub....
   */
  eos_main_area_props[PROP_ACTIONS] =
    g_param_spec_boolean ("actions", "Actions",
                         "Actions boolean stand in for future "
                         "object/widget/something",
                         FALSE,
                         G_PARAM_READABLE | G_PARAM_WRITABLE);

  g_object_class_install_properties (object_class, NPROPS, eos_main_area_props);
}

static void
eos_main_area_init (EosMainArea *self)
{
  gtk_widget_set_has_window(GTK_WIDGET(self), FALSE);
  self->priv = MAIN_AREA_PRIVATE (self);
}

/* Public API */

/**
 * eos_main_area_new:
 *
 * Creates a main area. It is invisible by default.
 *
 * Returns: a pointer to the main area widget.
 */
GtkWidget *
eos_main_area_new ()
{
  return GTK_WIDGET (g_object_new (EOS_TYPE_MAIN_AREA, NULL));
}


/**
 * eos_main_area_set_toolbar:
 * @main_area: a #EosMainArea
 * @toolbar: the toolbar widget to be displayed on left of content.
 *
 * Adds the toolbar widget to the main area. Passing %NULL will hide the
 * toolbar area.
 *
 * Return value: (transfer none):
 */
void *
eos_main_area_set_toolbar (EosMainArea *main_area, GtkWidget *toolbar)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (main_area));
  g_return_if_fail (toolbar == NULL || GTK_IS_WIDGET (toolbar));
  g_return_if_fail (toolbar == NULL || gtk_widget_get_parent (toolbar) == NULL);

  EosMainArea *self = EOS_MAIN_AREA (main_area);
  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *widget = GTK_WIDGET (main_area);

  if (priv->toolbar == toolbar)
    return;

  if (priv->toolbar)
    gtk_widget_unparent (priv->toolbar);

  priv->toolbar = toolbar;
  if (toolbar)
    gtk_widget_set_parent (toolbar, widget);

  if (gtk_widget_get_visible (widget))
    gtk_widget_queue_resize (widget);

  g_object_freeze_notify (G_OBJECT (main_area));
  g_object_notify (G_OBJECT (main_area), "toolbar");
  g_object_thaw_notify (G_OBJECT (main_area));
}

/**
 * eos_main_area_get_toolbar:
 * @main_area: a #EosMainArea
 *
 * Retrieves the toolbar widget for the main area.
 *
 * Return value: (transfer none): the toolbar widget,
 *     or %NULL if there is none
 */
GtkWidget *
eos_main_area_get_toolbar (EosMainArea *main_area)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (main_area), NULL);
  return main_area->priv->toolbar;
}

/**
 * eos_main_area_set_content:
 * @main_area: a #EosMainArea
 * @content: the content widget to be displayed in the center.
 *
 * Adds the content widget to the main area.
 *
 * Return value: (transfer none):
 */
void *
eos_main_area_set_content (EosMainArea *main_area, GtkWidget *content)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (main_area));
  g_return_if_fail (content == NULL || GTK_IS_WIDGET (content));
  g_return_if_fail (content == NULL || gtk_widget_get_parent (content) == NULL);

  EosMainArea *self = EOS_MAIN_AREA (main_area);
  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *widget = GTK_WIDGET (main_area);

  if (priv->content == content)
    return;

  if (priv->content)
    gtk_widget_unparent (priv->content);

  priv->content = content;
  if (content)
    gtk_widget_set_parent (content, widget);

  if (gtk_widget_get_visible (widget))
    gtk_widget_queue_resize (widget);

  g_object_freeze_notify (G_OBJECT (main_area));
  g_object_notify (G_OBJECT (main_area), "content");
  g_object_thaw_notify (G_OBJECT (main_area));
}

/**
 * eos_main_area_get_content:
 * @main_area: a #EosMainArea
 *
 * Retrieves the content widget for the main area.
 *
 * Return value: (transfer none): the content widget,
 *     or %NULL if there is none
 */
GtkWidget *
eos_main_area_get_content (EosMainArea *main_area)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (main_area), NULL);
  return main_area->priv->content;
}

/**
 * eos_main_area_set_actions:
 * @main_area: a #EosMainArea
 * @actions: %TRUE if there will be actions area on right of content.
 *
 * Sets whether an actions area should be displayed on the right of the
 * content. For now just a boolean eventually a widget.
 *
 * Return value: (transfer none):
 */
void *
eos_main_area_set_actions (EosMainArea *main_area, gboolean actions)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (main_area));
  EosMainArea *self = EOS_MAIN_AREA (main_area);
  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *widget = GTK_WIDGET (main_area);

  actions = actions != FALSE;

  if (priv->actions != actions) {
    priv->actions = actions;

    if (gtk_widget_get_visible (widget))
      gtk_widget_queue_resize (widget);

    g_object_notify (G_OBJECT (main_area), "actions");
  }
}

/**
 * eos_main_area_get_actions:
 * @main_area: a #EosMainArea
 *
 * Retrieves the actions widget for the main area.
 *
 * Return value: (transfer none): stand in actions boolean, for now.
 */
gboolean
eos_main_area_get_actions (EosMainArea *main_area)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (main_area), FALSE);
  return main_area->priv->actions;
}
