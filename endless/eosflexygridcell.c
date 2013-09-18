/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"

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

#if GLIB_CHECK_VERSION (2, 37, 5)

# define EOS_FLEXY_GRID_CELL_GET_PRIV(obj) \
  ((EosFlexyGridCellPrivate *) eos_flexy_grid_cell_get_instance_private ((EosFlexyGridCell *) (obj)))

G_DEFINE_TYPE_WITH_PRIVATE (EosFlexyGridCell, eos_flexy_grid_cell, GTK_TYPE_BIN)

#else

# define EOS_FLEXY_GRID_CELL_GET_PRIV(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EOS_TYPE_FLEXY_GRID_CELL, EosFlexyGridCellPrivate))

G_DEFINE_TYPE (EosFlexyGridCell, eos_flexy_grid_cell, GTK_TYPE_BIN)

#endif /* GLIB_CHECK_VERSION (2, 37, 5) */

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
#if !GLIB_CHECK_VERSION (2, 37, 6)
  g_type_class_add_private (klass, sizeof (EosFlexyGridCellPrivate));
#endif

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->set_property = eos_flexy_grid_cell_set_property;
  gobject_class->get_property = eos_flexy_grid_cell_get_property;

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

GtkWidget *
eos_flexy_grid_cell_new (void)
{
  return g_object_new (EOS_TYPE_FLEXY_GRID_CELL, NULL);
}

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
    }
}

EosFlexyShape
eos_flexy_grid_cell_get_shape (EosFlexyGridCell *cell)
{
  EosFlexyGridCellPrivate *priv;

  g_return_val_if_fail (EOS_IS_FLEXY_GRID_CELL (cell), EOS_FLEXY_SHAPE_SMALL);

  priv = cell->priv;

  return priv->shape;
}

/*< private >*/
void
eos_flexy_grid_cell_set_iter (EosFlexyGridCell *cell,
                              GSequenceIter    *iter)
{
  EosFlexyGridCellPrivate *priv = cell->priv;

  priv->iter = iter;
}

/*< private >*/
GSequenceIter *
eos_flexy_grid_cell_get_iter (EosFlexyGridCell *cell)
{
  EosFlexyGridCellPrivate *priv = cell->priv;

  return priv->iter;
}

/*< private >*/
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

gboolean
eos_flexy_grid_cell_get_selected (EosFlexyGridCell *cell)
{
  g_return_val_if_fail (EOS_IS_FLEXY_GRID_CELL (cell), FALSE);

  EosFlexyGridCellPrivate *priv = cell->priv;

  return priv->selected;
}
