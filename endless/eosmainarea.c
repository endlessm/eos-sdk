/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosmainarea-private.h"

#include <gtk/gtk.h>

/*
 * SECTION:main_area
 * @short_description: The main area for your application, under the top bar.
 * @title: EosMainArea
 *
 * EosMainArea has three areas it manages for you: the left toolbar area, the
 * right action area, and the center content area. They can be set with the
 * toolbar, actions and content properties. You will just about always want to
 * set the content widget to display something, but the toolbar and action
 * area will not appear unless set.
 */

G_DEFINE_TYPE (EosMainArea, eos_main_area, GTK_TYPE_CONTAINER)

#define MAIN_AREA_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_MAIN_AREA, EosMainAreaPrivate))

struct _EosMainAreaPrivate
{
  GtkWidget *toolbox;
  GtkWidget *content;
  GtkWidget *actions_standin;
  guint actions_visible : 1;
};

static GtkContainerClass *parent_klass = NULL;

static void
eos_main_area_get_preferred_width (GtkWidget *widget,
                                   gint      *minimal,
                                   gint      *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *toolbox = self->priv->toolbox;
  GtkWidget *content = self->priv->content;
  *minimal = *natural = 0;

  if ((toolbox && gtk_widget_get_visible (toolbox)) ||
      self->priv->actions_visible)
    {
      gint toolbox_minimal, toolbox_natural;
      gtk_widget_get_preferred_width (toolbox,
                                      &toolbox_minimal, &toolbox_natural);

      *minimal += 2 * toolbox_minimal;
      *natural += 2 * toolbox_natural;
    }

  if (content && gtk_widget_get_visible (content))
    {
      gint content_minimal, content_natural;
      gtk_widget_get_preferred_width (content,
                                      &content_minimal, &content_natural);

      *minimal += content_minimal;
      *natural += content_natural;
    }
}

static void
eos_main_area_get_preferred_height (GtkWidget *widget,
                                    gint      *minimal,
                                    gint      *natural)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *toolbox = self->priv->toolbox;
  GtkWidget *content = self->priv->content;
  *minimal = *natural = 0;

  if (toolbox && gtk_widget_get_visible (toolbox))
    {
      gint toolbox_minimal, toolbox_natural;
      gtk_widget_get_preferred_height (toolbox,
                                       &toolbox_minimal, &toolbox_natural);

      *minimal = MAX (*minimal, toolbox_minimal);
      *natural = MAX (*natural, toolbox_natural);
    }

  if (content && gtk_widget_get_visible (content))
    {
      gint content_minimal, content_natural;
      gtk_widget_get_preferred_height (content,
                                       &content_minimal, &content_natural);

      *minimal = MAX (*minimal, content_minimal);
      *natural = MAX (*natural, content_natural);
    }
}

// Don't size width for height or height for width, at least for now...
static void
eos_main_size_allocate (GtkWidget     *widget,
                        GtkAllocation *allocation)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);
  GtkWidget *toolbox = self->priv->toolbox;
  GtkWidget *content = self->priv->content;

  gtk_widget_set_allocation (widget, allocation);

  gint num_sidebars = 0;
  gboolean content_visible = content && gtk_widget_get_visible (content);
  gboolean toolbox_visible = toolbox && gtk_widget_get_visible (toolbox);
  if (toolbox_visible)
    num_sidebars++;
  if (self->priv->actions_visible)
    num_sidebars++;

  gint toolbox_min_width = 0, toolbox_nat_width = 0;
  gint toolbox_min_height, toolbox_nat_height;
  if (toolbox_visible)
    {
      gtk_widget_get_preferred_width (toolbox,
                                      &toolbox_min_width, &toolbox_nat_width);
      gtk_widget_get_preferred_height (toolbox,
                                       &toolbox_min_height, &toolbox_nat_height);
    }

  gint content_min_width = 0, content_nat_width = 0;
  gint content_min_height, content_nat_height;
  if (content_visible)
    {
      gtk_widget_get_preferred_width (content,
                                      &content_min_width, &content_nat_width);
      gtk_widget_get_preferred_height (content,
                                       &content_min_height, &content_nat_height);
    }

  /* Calculate width of sidebars. We allocate space in the following order:
  Sidebars min width, content min width, sidebars natural width, content
  natural width. */
  gint sidebar_width = 0;
  gint total_sidebars_width = allocation->width - content_min_width;
  if (num_sidebars > 0)
    {
      if (allocation->width < toolbox_min_width * num_sidebars)
        {
          sidebar_width = allocation->width / num_sidebars;
        }
      else if (content_visible &&
               total_sidebars_width < toolbox_nat_width * num_sidebars)
        {
          sidebar_width = total_sidebars_width / num_sidebars;
        }
      else
        {
          sidebar_width = toolbox_nat_width;
        }
    }

  // Allocate size
  gint x = allocation->x;
  gint y = allocation->y;
  if (toolbox_visible)
    {
      GtkAllocation toolbox_allocation;
      toolbox_allocation.x = x;
      toolbox_allocation.y = y;
      toolbox_allocation.width = sidebar_width;
      toolbox_allocation.height = MIN (toolbox_nat_height, allocation->height);
      gtk_widget_size_allocate (toolbox, &toolbox_allocation);
      x += toolbox_allocation.width;
    }
  if (self->priv->actions_visible)
    {
      GtkAllocation actions_allocation;
      actions_allocation.x = allocation->x + allocation->width - sidebar_width;
      actions_allocation.y = y;
      actions_allocation.width = sidebar_width;
      actions_allocation.height = allocation->height;
      gtk_widget_size_allocate (self->priv->actions_standin,
                                &actions_allocation);
    }
  if (content_visible)
    {
      GtkAllocation content_allocation;
      content_allocation.x = x;
      content_allocation.y = y;
      content_allocation.width = MAX (0,
                                      allocation->width - num_sidebars * sidebar_width);
      content_allocation.height = allocation->height;
      gtk_widget_size_allocate (content, &content_allocation);
      x += content_allocation.width;
    }
}

static void
eos_main_area_add (GtkContainer *container,
                   GtkWidget    *widget)
{
  EosMainArea *self = EOS_MAIN_AREA (container);

  /* Print out warning message?? */

  eos_main_area_set_content(self, widget);
}

static void
eos_main_area_remove (GtkContainer *container,
                      GtkWidget *widget)
{
  EosMainArea *self = EOS_MAIN_AREA(container);
  EosMainAreaPrivate *priv = self->priv;

  if (priv->content == widget)
    eos_main_area_set_content (self, NULL);
  else if (priv->toolbox == widget)
    eos_main_area_set_toolbox (self, NULL);
}

static void
eos_main_area_forall(GtkContainer *container,
                     gboolean      include_internals,
                     GtkCallback   callback,
                     gpointer      callback_data)
{
  EosMainArea *self = EOS_MAIN_AREA (container);
  EosMainAreaPrivate *priv = self->priv;

  if (priv->toolbox)
    (*callback) (priv->toolbox, callback_data);

  if (priv->content)
    (*callback) (priv->content, callback_data);

  if (include_internals && priv->actions_visible)
    (*callback) (priv->actions_standin, callback_data);
}

void
eos_main_destroy (GtkWidget *widget)
{
  EosMainArea *self = EOS_MAIN_AREA (widget);

  GTK_WIDGET_CLASS (parent_klass)->destroy (widget);
  gtk_widget_destroy (self->priv->actions_standin);
}

static void
eos_main_area_class_init (EosMainAreaClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  parent_klass = g_type_class_peek_parent (klass);

  g_type_class_add_private (klass, sizeof (EosMainAreaPrivate));

  widget_class->get_preferred_width = eos_main_area_get_preferred_width;
  widget_class->get_preferred_height = eos_main_area_get_preferred_height;
  widget_class->size_allocate = eos_main_size_allocate;
  widget_class->destroy = eos_main_destroy;

  container_class->forall = eos_main_area_forall;
  container_class->add = eos_main_area_add;
  container_class->remove = eos_main_area_remove;
}

static void
eos_main_area_init (EosMainArea *self)
{
  gtk_widget_set_has_window(GTK_WIDGET(self), FALSE);
  self->priv = MAIN_AREA_PRIVATE (self);

  self->priv->actions_standin = gtk_event_box_new ();
  g_object_ref_sink (self->priv->actions_standin);
  GdkRGBA red = { 1.0, 0.0, 0.0, 1.0 };
  gtk_widget_override_background_color (self->priv->actions_standin,
                                        GTK_STATE_FLAG_NORMAL,
                                        &red);
  gtk_widget_show (self->priv->actions_standin);
}

/* Internal Public API */

/*
 * eos_main_area_new:
 *
 * Creates a main area. It is invisible by default.
 *
 * Returns: a pointer to the main area widget.
 */
GtkWidget *
eos_main_area_new (void)
{
  return GTK_WIDGET (g_object_new (EOS_TYPE_MAIN_AREA, NULL));
}


/*
 * eos_main_area_set_toolbox:
 * @self: a #EosMainArea
 * @toolbox: the toolbox widget to be displayed on left of content.
 *
 * Adds the toolbox widget to the main area. Passing %NULL will hide the
 * toolbox area.
 */
void
eos_main_area_set_toolbox (EosMainArea *self,
                           GtkWidget   *toolbox)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (self));
  g_return_if_fail (toolbox == NULL || GTK_IS_WIDGET (toolbox));

  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *self_widget = GTK_WIDGET (self);

  if (priv->toolbox == toolbox)
    return;

  if (priv->toolbox)
    gtk_widget_unparent (priv->toolbox);

  priv->toolbox = toolbox;
  if (toolbox)
    {
      gtk_widget_set_parent (toolbox, self_widget);
      gtk_widget_show (toolbox);
    }

  if (gtk_widget_get_visible (self_widget))
    gtk_widget_queue_resize (self_widget);
}

/*
 * eos_main_area_get_toolbox:
 * @self: a #EosMainArea
 *
 * Retrieves the toolbox widget for the main area.
 *
 * Return value: (transfer none): the toolbox widget,
 *     or %NULL if there is none
 */
GtkWidget *
eos_main_area_get_toolbox (EosMainArea *self)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (self), NULL);
  return self->priv->toolbox;
}

/*
 * eos_main_area_set_content:
 * @self: a #EosMainArea
 * @content: the content widget to be displayed in the center.
 *
 * Adds the content widget to the main area.
 */
void
eos_main_area_set_content (EosMainArea *self, GtkWidget *content)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (self));
  g_return_if_fail (content == NULL || GTK_IS_WIDGET (content));
  g_return_if_fail (content == NULL || gtk_widget_get_parent (content) == NULL);

  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *self_widget = GTK_WIDGET (self);

  if (priv->content == content)
    return;

  if (priv->content)
    gtk_widget_unparent (priv->content);

  priv->content = content;
  if (content)
    gtk_widget_set_parent (content, self_widget);

  if (gtk_widget_get_visible (self_widget))
    gtk_widget_queue_resize (self_widget);
}

/*
 * eos_main_area_get_content:
 * @self: a #EosMainArea
 *
 * Retrieves the content widget for the main area.
 *
 * Return value: (transfer none): the content widget,
 *     or %NULL if there is none
 */
GtkWidget *
eos_main_area_get_content (EosMainArea *self)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (self), NULL);
  return self->priv->content;
}

/*
 * eos_main_area_set_actions:
 * @self: a #EosMainArea
 * @actions: %TRUE if there will be actions area on right of content.
 *
 * Sets whether an actions area should be displayed on the right of the
 * content. For now just a boolean eventually a widget/list of actions or
 * something.
 */
void
eos_main_area_set_actions (EosMainArea *self, gboolean actions_visible)
{
  g_return_if_fail (EOS_IS_MAIN_AREA (self));

  EosMainAreaPrivate *priv = self->priv;
  GtkWidget *self_widget = GTK_WIDGET (self);

  actions_visible = actions_visible != FALSE;

  if (priv->actions_visible == actions_visible)
    return;

  priv->actions_visible = actions_visible;

  if (priv->actions_visible)
    gtk_widget_set_parent (priv->actions_standin, self_widget);
  else
    gtk_widget_unparent (priv->actions_standin);
}

/**
 * eos_main_area_get_actions:
 * @self: a #EosMainArea
 *
 * Retrieves the actions boolean value. See set_actions.
 *
 * Return value: stand in actions boolean, for now.
 */
gboolean
eos_main_area_get_actions (EosMainArea *self)
{
  g_return_val_if_fail (EOS_IS_MAIN_AREA (self), FALSE);
  return self->priv->actions_visible;
}
