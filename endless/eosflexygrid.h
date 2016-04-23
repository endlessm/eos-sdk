/* Copyright 2013-2016 Endless Mobile, Inc. */

#ifndef EOS_FLEXY_GRID_H
#define EOS_FLEXY_GRID_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_FLEXY_GRID                     (eos_flexy_grid_get_type ())
#define EOS_FLEXY_GRID(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOS_TYPE_FLEXY_GRID, EosFlexyGrid))
#define EOS_IS_FLEXY_GRID(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOS_TYPE_FLEXY_GRID))
#define EOS_FLEXY_GRID_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), EOS_TYPE_FLEXY_GRID, EosFlexyGridClass))
#define EOS_IS_FLEXY_GRID_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), EOS_TYPE_FLEXY_GRID))
#define EOS_FLEXY_GRID_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), EOS_TYPE_FLEXY_GRID, EosFlexyGridClass))

#define EOS_TYPE_FLEXY_GRID_CELL                (eos_flexy_grid_cell_get_type ())
#define EOS_FLEXY_GRID_CELL(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), EOS_TYPE_FLEXY_GRID_CELL, EosFlexyGridCell))
#define EOS_IS_FLEXY_GRID_CELL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EOS_TYPE_FLEXY_GRID_CELL))
#define EOS_FLEXY_GRID_CELL_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), EOS_TYPE_FLEXY_GRID_CELL, EosFlexyGridCellClass))
#define EOS_IS_FLEXY_GRID_CELL_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), EOS_TYPE_FLEXY_GRID_CELL))
#define EOS_FLEXY_GRID_CELL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), EOS_TYPE_FLEXY_GRID_CELL, EosFlexyGridCellClass))

/**
 * EOS_STYLE_CLASS_FLEXY_GRID:
 *
 * A CSS class to match a grid with flexible layout, used by #EosFlexyGrid.
 *
 * Deprecated: 0.2: The #EosFlexyGrid widget should not be used in newly written code
 */
#define EOS_STYLE_CLASS_FLEXY_GRID      "flexy-grid"
/**
 * EOS_STYLE_CLASS_FLEXY_GRID_CELL:
 *
 * A CSS class to match a cell inside a grid with flexible layout, used by
 * #EosFlexyGrid.
 *
 * Deprecated: 0.2: The #EosFlexyGrid widget should not be used in newly written code
 */
#define EOS_STYLE_CLASS_FLEXY_GRID_CELL "flexy-grid-cell"

/**
 * EosFlexyGrid:
 *
 * This structure contains no public members.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
typedef struct _EosFlexyGrid            EosFlexyGrid;
typedef struct _EosFlexyGridClass       EosFlexyGridClass;

/**
 * EosFlexyGridCell:
 *
 * This structure contains no public members.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
typedef struct _EosFlexyGridCell        EosFlexyGridCell;
typedef struct _EosFlexyGridCellClass   EosFlexyGridCellClass;

/**
 * EosFlexyGridSortFunc:
 * @cell_a: first cell to compare
 * @cell_b: second cell to compare
 * @user_data: data passed to eos_flexy_grid_set_sort_func()
 *
 * Type for functions that determine the sort order of two cells inside the
 * grid.
 *
 * Returns: -1 if @cell_a should come before @cell_b, 1 if @cell_a should come
 * after @cell_b, or 0 if @cell_a and @cell_b are identical.
 *
 * Deprecated: 0.2: Use a #GtkGrid instead
 */
typedef gint (* EosFlexyGridSortFunc) (EosFlexyGridCell *cell_a,
                                       EosFlexyGridCell *cell_b,
                                       gpointer          user_data);

struct _EosFlexyGrid
{
  /*< private >*/
  GtkContainer parent_instance;
};

struct _EosFlexyGridClass
{
  /*< private >*/
  GtkContainerClass parent_class;

  void (* cell_selected)  (EosFlexyGrid     *grid,
                           EosFlexyGridCell *cell);
  void (* cell_activated) (EosFlexyGrid     *grid,
                           EosFlexyGridCell *cell);

  gpointer _padding[8];
};

EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
GType eos_flexy_grid_get_type (void) G_GNUC_CONST;

EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
GtkWidget *             eos_flexy_grid_new                      (void);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
void                    eos_flexy_grid_set_cell_size            (EosFlexyGrid         *grid,
                                                                 int                   size);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
guint                   eos_flexy_grid_get_cell_size            (EosFlexyGrid         *grid);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
void                    eos_flexy_grid_set_cell_spacing         (EosFlexyGrid         *grid,
                                                                 int                   spacing);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
guint                   eos_flexy_grid_get_cell_spacing         (EosFlexyGrid         *grid);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
void                    eos_flexy_grid_insert                   (EosFlexyGrid         *grid,
                                                                 GtkWidget            *child,
                                                                 int                   index_);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
void                    eos_flexy_grid_set_sort_func            (EosFlexyGrid         *grid,
                                                                 EosFlexyGridSortFunc  sort_func,
                                                                 gpointer              data,
                                                                 GDestroyNotify        notify);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
EosFlexyGridCell *      eos_flexy_grid_get_cell_at_coords       (EosFlexyGrid         *grid,
                                                                 double                x_pos,
                                                                 double                y_pos);

struct _EosFlexyGridCell
{
  /*< private >*/
  GtkBin parent_instance;

  gpointer priv;
};

struct _EosFlexyGridCellClass
{
  /*< private >*/
  GtkBinClass parent_class;

  gpointer _padding[8];
};

EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
GType eos_flexy_grid_cell_get_type (void) G_GNUC_CONST;

EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
GtkWidget *     eos_flexy_grid_cell_new                 (void);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
void            eos_flexy_grid_cell_set_shape           (EosFlexyGridCell *cell,
                                                         EosFlexyShape     shape);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
EosFlexyShape   eos_flexy_grid_cell_get_shape           (EosFlexyGridCell *cell);
EOS_SDK_DEPRECATED_IN_0_2_FOR (GtkGrid)
gboolean        eos_flexy_grid_cell_get_selected        (EosFlexyGridCell *cell);

G_END_DECLS

#endif /* EOS_FLEXY_GRID_H */
