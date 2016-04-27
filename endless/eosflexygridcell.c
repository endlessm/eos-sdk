/* Copyright (C) 2013-2016 Endless Mobile, Inc. */

#include "config.h"

#define EOS_SDK_DISABLE_DEPRECATION_WARNINGS
#include "eosflexygrid-private.h"

#include <glib-object.h>
#include <gtk/gtk.h>

EOS_DEFINE_ENUM_TYPE (EosFlexyShape, eos_flexy_shape,
                      EOS_ENUM_VALUE (EOS_FLEXY_SHAPE_SMALL, small)
                      EOS_ENUM_VALUE (EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL, medium-horizontal)
                      EOS_ENUM_VALUE (EOS_FLEXY_SHAPE_MEDIUM_VERTICAL, medium-vertical)
                      EOS_ENUM_VALUE (EOS_FLEXY_SHAPE_LARGE, large))

typedef struct {
  EosFlexyShape shape;

  GSequenceIter *iter;

  guint selected : 1;
} EosFlexyGridCellPrivate;

#define EOS_FLEXY_GRID_CELL_GET_PRIV(obj) \
  ((EosFlexyGridCellPrivate *) eos_flexy_grid_cell_get_instance_private ((EosFlexyGridCell *) (obj)))

G_DEFINE_TYPE_WITH_PRIVATE (EosFlexyGridCell, eos_flexy_grid_cell, GTK_TYPE_BIN)

enum
{
  PROP_0,

  PROP_SHAPE,

  PROP_LAST
};

static GParamSpec *obj_props[PROP_LAST] = { NULL, };

static void
eos_flexy_grid_cell_set_property (GObject      *gobject,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  EosFlexyGridCell *self = EOS_FLEXY_GRID_CELL (gobject);

  switch (prop_id)
    {
    case PROP_SHAPE:
      eos_flexy_grid_cell_set_shape (self, g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
eos_flexy_grid_cell_get_property (GObject    *gobject,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  EosFlexyGridCell *self = EOS_FLEXY_GRID_CELL (gobject);
  EosFlexyGridCellPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_SHAPE:
      g_value_set_enum (value, priv->shape);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
eos_flexy_grid_cell_class_init (EosFlexyGridCellClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->set_property = eos_flexy_grid_cell_set_property;
  gobject_class->get_property = eos_flexy_grid_cell_get_property;

  /**
   * EosFlexyGridCell:shape:
   *
   * The shape of the cell.
   */
  obj_props[PROP_SHAPE] =
    g_param_spec_enum ("shape",
                       "Shape",
                       "The shape of the cell",
                       EOS_TYPE_FLEXY_SHAPE,
                       EOS_FLEXY_SHAPE_SMALL,
                       G_PARAM_STATIC_STRINGS |
                       G_PARAM_READWRITE);

  g_object_class_install_properties (gobject_class, PROP_LAST, obj_props);
}

static void
eos_flexy_grid_cell_init (EosFlexyGridCell *self)
{
  EosFlexyGridCellPrivate *priv = EOS_FLEXY_GRID_CELL_GET_PRIV (self);

  priv->shape = EOS_FLEXY_SHAPE_SMALL;

  self->priv = priv;

  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, EOS_STYLE_CLASS_FLEXY_GRID_CELL);
}

/**
 * eos_flexy_grid_cell_new:
 *
 * Creates a new #EosFlexyGridCell widget.
 *
 * Return value: (transfer full): the newly created #EosFlexyGridCell widget
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
GtkWidget *
eos_flexy_grid_cell_new (void)
{
  return g_object_new (EOS_TYPE_FLEXY_GRID_CELL, NULL);
}

/**
 * eos_flexy_grid_cell_set_shape:
 * @cell: a #EosFlexyGridCell
 * @shape: the shape of the cell
 *
 * Sets the shape of the @cell. The @shape determines the amount of
 * space inside a #EosFlexyGrid that will be assigned to the @cell.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
void
eos_flexy_grid_cell_set_shape (EosFlexyGridCell *cell,
                               EosFlexyShape     shape)
{
  EosFlexyGridCellPrivate *priv;

  g_return_if_fail (EOS_IS_FLEXY_GRID_CELL (cell));

  priv = cell->priv;
  if (priv->shape != shape)
    {
      priv->shape = shape;

      g_object_notify_by_pspec (G_OBJECT (cell), obj_props[PROP_SHAPE]);

      gtk_widget_queue_resize (GTK_WIDGET (cell));
    }
}

/**
 * eos_flexy_grid_cell_get_shape:
 * @cell: a #EosFlexyGridCell
 *
 * Retrieves the shape of @cell.
 *
 * Return value: the shape of the #EosFlexyGridCell
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
EosFlexyShape
eos_flexy_grid_cell_get_shape (EosFlexyGridCell *cell)
{
  EosFlexyGridCellPrivate *priv;

  g_return_val_if_fail (EOS_IS_FLEXY_GRID_CELL (cell), EOS_FLEXY_SHAPE_SMALL);

  priv = cell->priv;

  return priv->shape;
}

/*< private >
 * eos_flexy_grid_cell_set_iter:
 * @cell: a #EosFlexyGridCell
 * @iter: a #GSequenceIter
 *
 * The #GSequenceIter associated to @cell.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
void
eos_flexy_grid_cell_set_iter (EosFlexyGridCell *cell,
                              GSequenceIter    *iter)
{
  EosFlexyGridCellPrivate *priv = cell->priv;

  priv->iter = iter;
}

/*< private >
 * eos_flexy_grid_cell_get_iter:
 * @cell: a #EosFlexyGridCell
 *
 * Retrieves the #GSequenceIter associated to @cell.
 *
 * Return value: (transfer none): a #GSequenceIter
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
GSequenceIter *
eos_flexy_grid_cell_get_iter (EosFlexyGridCell *cell)
{
  EosFlexyGridCellPrivate *priv = cell->priv;

  return priv->iter;
}

/*< private >
 * eos_flexy_grid_cell_set_selected:
 * @cell: a #EosFlexyGridCell
 * @selected: %TRUE to select the @cell, and %FALSE otherwise
 *
 * Sets whether the @cell should be selected or not.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
void
eos_flexy_grid_cell_set_selected (EosFlexyGridCell *cell,
                                  gboolean          selected)
{
  g_return_if_fail (EOS_IS_FLEXY_GRID_CELL (cell));

  EosFlexyGridCellPrivate *priv = cell->priv;

  selected = !!selected;
  if (priv->selected != selected)
    {
      priv->selected = selected;
    }
}

/**
 * eos_flexy_grid_cell_get_selected:
 * @cell: a #EosFlexyGridCell
 *
 * Checks whether @cell is selected.
 *
 * Return value: %TRUE if the #EosFlexyGridCell is selected,
 *   and %FALSE otherwise
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
gboolean
eos_flexy_grid_cell_get_selected (EosFlexyGridCell *cell)
{
  g_return_val_if_fail (EOS_IS_FLEXY_GRID_CELL (cell), FALSE);

  EosFlexyGridCellPrivate *priv = cell->priv;

  return priv->selected;
}
