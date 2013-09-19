/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"

#include "eosflexygrid-private.h"

#include <glib-object.h>
#include <gtk/gtk.h>

/*#define VERBOSE 1*/

#define DEFAULT_CELL_SIZE       180
#define DEFAULT_SPACING         15

#ifdef VERBOSE
#define DEBUG(x)        x
#define CELL_DEBUG_SIZE(_cells,_i)      \
  g_print ("Cell[%u] = { .cell = %p, .x = %d, .y = %d, .width = %d, .height = %d }\n", \
           _i, \
           _cells[_i].cell, \
           _cells[_i].cell_x, _cells[_i].cell_y, _cells[_i].cell_width, _cells[_i].cell_height)
#else
#define DEBUG(x)
#define CELL_DEBUG_SIZE(cells,i)
#endif

typedef struct {
  GSequence *children;

  EosFlexyGridSortFunc sort_func;
  gpointer sort_data;
  GDestroyNotify sort_notify;

  GtkAdjustment *adjustment;

  int cell_size;
  int cell_spacing;

  int n_visible_children;

  EosFlexyGridCell *prelight_cell;
  EosFlexyGridCell *active_cell;

  guint in_widget : 1;
} EosFlexyGridPrivate;

enum
{
  CELL_SELECTED,
  CELL_ACTIVATED,

  LAST_SIGNAL
};

#if GLIB_CHECK_VERSION (2, 37, 5)

# define EOS_FLEXY_GRID_GET_PRIV(obj) \
  ((EosFlexyGridPrivate *) eos_flexy_grid_get_instance_private ((EosFlexyGrid *) (obj)))

G_DEFINE_TYPE_WITH_PRIVATE (EosFlexyGrid, eos_flexy_grid, GTK_TYPE_CONTAINER)

#else

# define EOS_FLEXY_GRID_GET_PRIV(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), EOS_TYPE_FLEXY_GRID, EosFlexyGridPrivate))

G_DEFINE_TYPE (EosFlexyGrid, eos_flexy_grid, GTK_TYPE_CONTAINER)

#endif /* GLIB_CHECK_VERSION (2, 37, 5) */

static guint grid_signals[LAST_SIGNAL] = { 0, };

static EosFlexyGridCell *
eos_flexy_grid_get_cell_at_coords (EosFlexyGrid *grid,
                                   double        x_pos,
                                   double        y_pos)
{
  EosFlexyGridPrivate *priv = grid->priv;
  GSequenceIter *iter;

  /* naive hit detection */
  for (iter = g_sequence_get_begin_iter (priv->children);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      GtkWidget *widget = g_sequence_get (iter);
      GtkAllocation allocation;

      gtk_widget_get_allocation (widget, &allocation);

      if (x_pos >= allocation.x && x_pos < allocation.x + allocation.width &&
          y_pos >= allocation.y && y_pos < allocation.y + allocation.height)
        return EOS_FLEXY_GRID_CELL (widget);
    }

  return NULL;
}

static void
eos_flexy_grid_update_cell_prelight (EosFlexyGrid *grid,
                                     EosFlexyGridCell *cell)
{
  EosFlexyGridPrivate *priv = grid->priv;

  if (cell == priv->prelight_cell)
    return;

  if (priv->prelight_cell != NULL)
    {
      gtk_widget_unset_state_flags (GTK_WIDGET (priv->prelight_cell),
                                    GTK_STATE_FLAG_PRELIGHT);
      eos_flexy_grid_cell_set_selected (priv->prelight_cell, FALSE);
    }

  if (cell != NULL && gtk_widget_is_sensitive (GTK_WIDGET (cell)))
    {
      priv->prelight_cell = cell;
      gtk_widget_set_state_flags (GTK_WIDGET (priv->prelight_cell),
                                  GTK_STATE_FLAG_PRELIGHT,
                                  FALSE);
      eos_flexy_grid_cell_set_selected (cell, TRUE);

      g_signal_emit (grid, grid_signals[CELL_SELECTED], 0, cell);
    }
  else
    priv->prelight_cell = NULL;

  gtk_widget_queue_draw (GTK_WIDGET (grid));
}

typedef struct _CellRequest
{
  EosFlexyGridCell *cell;

  int cell_x;
  int cell_y;
  int cell_width;
  int cell_height;
} CellRequest;

static inline void
reset_cells (CellRequest *cells,
             guint        n_cells)
{
  for (guint i = 0; i < n_cells; i++)
    {
      cells[i].cell = NULL;
      cells[i].cell_x = 0;
      cells[i].cell_y = 0;
      cells[i].cell_width = 0;
      cells[i].cell_height = 0;
    }
}

static inline guint
mark_cell (CellRequest      *cells,
           guint             n_cells,
           guint             stride,
           guint             index_,
           guint             hspan,
           guint             vspan,
           EosFlexyGridCell *cell)
{
  guint i;

  cells[index_].cell = cell;

  i = 0;
  while (i++ < vspan - 1)
    {
      DEBUG (g_print ("Marking vspan cell %u\n", index_ + (stride * i)));
      cells[index_ + (stride * i)].cell = cell;
    }

  i = 0;
  while (i++ < hspan - 1)
    {
      DEBUG (g_print ("Marking hspan cell %u\n", index_ + i));
      cells[index_ + i].cell = cell;
    }

  guint next_slot = index_ + hspan;
  while (next_slot < n_cells)
    {
      DEBUG (g_print ("next_slot: %u (index_: %u + hspan: %u)\n", next_slot, index_, hspan));
      if (cells[next_slot].cell == NULL)
        break;

      next_slot += 1;
    }

  return next_slot;
}

static void
eos_flexy_grid_distribute_cell_request (CellRequest      *cells,
                                        guint             n_cells,
                                        guint             n_columns,
                                        EosFlexyGridCell *cell,
                                        guint             cur_index,
                                        int               cell_size,
                                        guint            *next_index)
{
  EosFlexyShape cell_shape = eos_flexy_grid_cell_get_shape (cell);

  switch (cell_shape)
    {
    case EOS_FLEXY_SHAPE_SMALL:
      /* b1 */
      cells[cur_index].cell_width = cell_size;
      cells[cur_index].cell_height = cell_size;
      cells[cur_index].cell_x = cur_index % n_columns * cell_size;
      cells[cur_index].cell_y = cur_index / n_columns * cell_size;

      *next_index = mark_cell (cells, n_cells, n_columns,
                               cur_index,
                               1, 1,
                               cell);
      CELL_DEBUG_SIZE (cells, cur_index);
      break;

    case EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL:
      /* b2h */
      cells[cur_index].cell_width = cell_size * 2;
      cells[cur_index].cell_height = cell_size;
      cells[cur_index].cell_x = cur_index % n_columns * cell_size;
      cells[cur_index].cell_y = cur_index / n_columns * cell_size;

      *next_index = mark_cell (cells, n_cells, n_columns,
                               cur_index,
                               2, 1,
                               cell);
      CELL_DEBUG_SIZE (cells, cur_index);
      break;

    case EOS_FLEXY_SHAPE_MEDIUM_VERTICAL:
      /* b2v */
      cells[cur_index].cell_width = cell_size;
      cells[cur_index].cell_height = cell_size * 2;
      cells[cur_index].cell_x = cur_index % n_columns * cell_size;
      cells[cur_index].cell_y = cur_index / n_columns * cell_size;

      *next_index = mark_cell (cells, n_cells, n_columns,
                               cur_index,
                               1, 2,
                               cell);
      CELL_DEBUG_SIZE (cells, cur_index);
      break;

    case EOS_FLEXY_SHAPE_LARGE:
      /* b4 */
      cells[cur_index].cell_width = cell_size * 2;
      cells[cur_index].cell_height = cell_size * 2;
      cells[cur_index].cell_x = cur_index % n_columns * cell_size;
      cells[cur_index].cell_y = cur_index / n_columns * cell_size;

      *next_index = mark_cell (cells, n_cells, n_columns,
                               cur_index,
                               2, 2,
                               cell);
      CELL_DEBUG_SIZE (cells, cur_index);
      break;
    }
}

static GtkSizeRequestMode
eos_flexy_grid_get_request_mode (GtkWidget *widget)
{
  return GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH;
}

static void
eos_flexy_grid_get_preferred_width (GtkWidget *widget,
                                    gint      *minimum_width_out,
                                    gint      *natural_width_out)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (widget)->priv;
  int minimum_width, natural_width;

  int target_column_size = priv->cell_size < 0 ? DEFAULT_CELL_SIZE : priv->cell_size;

  /* minimum width: the biggest possible cell width */
  minimum_width = target_column_size * 2;

  int width = 0;

  /* natural width: the maximum width of all the cells on a single row */
  GSequenceIter *iter;
  for (iter = g_sequence_get_begin_iter (priv->children);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      EosFlexyGridCell *cell = g_sequence_get (iter);

      if (!gtk_widget_get_visible (GTK_WIDGET (cell)))
        continue;

      EosFlexyShape cell_shape = eos_flexy_grid_cell_get_shape (cell);

      switch (cell_shape)
        {
        case EOS_FLEXY_SHAPE_SMALL:
          /* b1 */
          width += target_column_size;
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL:
          /* b2h */
          width += target_column_size * 2;
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_VERTICAL:
          /* b2v */
          width += target_column_size;
          break;

        case EOS_FLEXY_SHAPE_LARGE:
          /* b4 */
          width += target_column_size * 2;
          break;
        }
    }

  natural_width = width;

  if (minimum_width_out)
    *minimum_width_out = minimum_width;
  if (natural_width_out)
    * natural_width_out = MAX (natural_width, minimum_width);
}

static void
eos_flexy_grid_get_preferred_width_for_height (GtkWidget *widget,
                                               gint       for_height,
                                               gint      *minimum_width,
                                               gint      *natural_width)
{
  eos_flexy_grid_get_preferred_width (widget, minimum_width, natural_width);
}

static void
eos_flexy_grid_get_preferred_height_for_width (GtkWidget *widget,
                                               gint       for_width,
                                               gint      *minimum_height_out,
                                               gint      *natural_height_out)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (widget)->priv;
  int minimum_height, natural_height;

  int cell_size = priv->cell_size < 0 ? DEFAULT_CELL_SIZE : priv->cell_size;

  /* minimum height: the maximum height of all the cells on a single row */
  minimum_height = cell_size * 2;

  int max_row_width = for_width;
  int row_width = 0, row_height = cell_size;

  int height = row_height;
  guint row = 1;

  /* natural width: the maximum width of all the cells on a single row */
  GSequenceIter *iter;
  for (iter = g_sequence_get_begin_iter (priv->children);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      EosFlexyGridCell *cell = g_sequence_get (iter);
      int cell_height = 0, cell_width = 0;

      if (!gtk_widget_get_visible (GTK_WIDGET (cell)))
        continue;

      EosFlexyShape cell_shape = eos_flexy_grid_cell_get_shape (cell);

      switch (cell_shape)
        {
        case EOS_FLEXY_SHAPE_SMALL:
          /* b1 */
          cell_width = cell_size;
          cell_height = cell_size;
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL:
          /* b2h */
          cell_width = cell_size * 2;
          cell_height = cell_size;
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_VERTICAL:
          /* b2v */
          cell_width = cell_size;
          cell_height = cell_size * 2;
          break;

        case EOS_FLEXY_SHAPE_LARGE:
          /* b4 */
          cell_width = cell_size * 2;
          cell_height = cell_size * 2;
          break;
        }

      row_width += cell_width;

      if (row_width > max_row_width)
        {
          height = row * row_height;

          row += 1;
          row_width = cell_width;
          row_height = MAX (cell_height, cell_size);
        }
      else
        row_height = MAX (row_height, cell_height);
    }

  natural_height = MAX (height, cell_size);

  if (minimum_height_out)
    *minimum_height_out = minimum_height;
  if (natural_height_out)
    *natural_height_out = MAX (natural_height, minimum_height);
}

static void
eos_flexy_grid_get_preferred_height (GtkWidget *widget,
                                     gint      *minimum_height,
                                     gint      *natural_height)
{
  gint natural_width = 0;

  eos_flexy_grid_get_preferred_width (widget, NULL, &natural_width);
  eos_flexy_grid_get_preferred_height_for_width (widget, natural_width, minimum_height, natural_height);
}

static void
eos_flexy_grid_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  gtk_widget_set_allocation (widget, allocation);

  GdkWindow *window = gtk_widget_get_window (widget);
  if (window != NULL)
    gdk_window_move_resize (window,
                            allocation->x,
                            allocation->y,
                            allocation->width,
                            allocation->height);

  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (widget)->priv;

  int target_column_size = priv->cell_size < 0 ? DEFAULT_CELL_SIZE : priv->cell_size;
  int available_width = allocation->width;
  int available_height = allocation->height;
  int cell_size = target_column_size;
  int n_columns = MAX (available_width / (cell_size + priv->cell_spacing), 2);
  int n_rows = MAX (available_height / (cell_size + priv->cell_spacing), 1);

  guint cur_index = 0, next_index = 0;
  guint n_cells = n_columns * n_rows;
  CellRequest *cells = g_newa (CellRequest, n_cells);

  reset_cells (cells, n_cells);

  DEBUG (g_print ("Size; %d x %d (cols: %u, rows: %u)\n",
                  available_width, available_height,
                  n_columns,
                  n_rows));

  GSequenceIter *iter;
  for (iter = g_sequence_get_begin_iter (priv->children);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      EosFlexyGridCell *cell = g_sequence_get (iter);
      GtkAllocation child_allocation;

      if (!gtk_widget_get_visible (GTK_WIDGET (cell)))
        continue;

      if (cur_index > n_cells)
        break;

      eos_flexy_grid_distribute_cell_request (cells, n_cells, n_columns,
                                              cell,
                                              cur_index,
                                              cell_size,
                                              &next_index);

      child_allocation.x = cells[cur_index].cell_x + priv->cell_spacing;
      child_allocation.y = cells[cur_index].cell_y + priv->cell_spacing;
      child_allocation.width = cells[cur_index].cell_width - priv->cell_spacing;
      child_allocation.height = cells[cur_index].cell_height - priv->cell_spacing;

      gtk_widget_size_allocate (GTK_WIDGET (cell), &child_allocation);

      DEBUG (g_print ("cur_index: %u, next_index: %u, column: %d, row: %d\n",
                      cur_index, next_index,
                      cur_index % n_columns,
                      cur_index / n_columns));

      cur_index = next_index;
    }
}

static void
eos_flexy_grid_realize (GtkWidget *widget)
{
  GtkAllocation allocation;
  gtk_widget_get_allocation (widget, &allocation);
  gtk_widget_set_realized (widget, TRUE);

  GdkWindowAttr attributes = { 0, };
  attributes.x = allocation.x;
  attributes.y = allocation.y;
  attributes.width = allocation.width;
  attributes.height = allocation.height;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget)
                        | GDK_ENTER_NOTIFY_MASK
                        | GDK_LEAVE_NOTIFY_MASK
                        | GDK_POINTER_MOTION_MASK
                        | GDK_EXPOSURE_MASK
                        | GDK_BUTTON_PRESS_MASK
                        | GDK_BUTTON_RELEASE_MASK;
  attributes.wclass = GDK_INPUT_OUTPUT;

  GdkWindow *window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                      &attributes,
                                      GDK_WA_X | GDK_WA_Y);
  gtk_style_context_set_background (gtk_widget_get_style_context (widget), window);
  gdk_window_set_user_data (window, (GObject *) widget);
  gtk_widget_set_window (widget, window); /* Passes ownership */
}

static gboolean
eos_flexy_grid_draw (GtkWidget *widget,
                     cairo_t   *cr)
{
  GtkAllocation allocation;
  gtk_widget_get_allocation (widget, &allocation);

  GtkStyleContext *context;
  context = gtk_widget_get_style_context (widget);
  gtk_render_background (context, cr, 0, 0, allocation.width, allocation.height);
  gtk_render_frame (context, cr, 0, 0, allocation.width, allocation.height);

  GTK_WIDGET_CLASS (eos_flexy_grid_parent_class)->draw (widget, cr);

  return TRUE;
}

static void
eos_flexy_grid_add (GtkContainer *container,
                    GtkWidget    *child)
{
  eos_flexy_grid_insert (EOS_FLEXY_GRID (container), child, -1);
}

static void
eos_flexy_grid_remove (GtkContainer *container,
                       GtkWidget    *widget)
{
  if (!EOS_IS_FLEXY_GRID_CELL (widget))
    {
      g_critical ("Trying to remove non-child %p", widget);
      return;
    }

  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (container)->priv;

  EosFlexyGridCell *cell = EOS_FLEXY_GRID_CELL (widget);
  GSequenceIter *iter = eos_flexy_grid_cell_get_iter (cell);
  if (iter == NULL || g_sequence_iter_get_sequence (iter) != priv->children)
    {
      g_critical ("The cell %p is not associated to the grid %p", cell, container);
      return;
    }

  gboolean was_visible = gtk_widget_get_visible (widget);

  if (cell == priv->prelight_cell)
    {
      gtk_widget_unset_state_flags (GTK_WIDGET (priv->prelight_cell),
                                    GTK_STATE_FLAG_PRELIGHT);
      priv->prelight_cell = NULL;
    }

  if (cell == priv->active_cell)
    {
      gtk_widget_unset_state_flags (GTK_WIDGET (priv->active_cell),
                                    GTK_STATE_FLAG_ACTIVE);
      priv->active_cell = NULL;
    }

  gtk_widget_unparent (widget);
  g_sequence_remove (iter);

  if (was_visible && gtk_widget_get_visible (GTK_WIDGET (container)))
    gtk_widget_queue_resize (GTK_WIDGET (container));
}

static void
eos_flexy_grid_forall (GtkContainer *container,
                       gboolean      internals,
                       GtkCallback   callback,
                       gpointer      data)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (container)->priv;

  GSequenceIter *iter = g_sequence_get_begin_iter (priv->children);
  while (!g_sequence_iter_is_end (iter))
    {
      GtkWidget *child = g_sequence_get (iter);
      iter = g_sequence_iter_next (iter);

      callback (child, data);
    }
}

static GType
eos_flexy_grid_child_type (GtkContainer *container)
{
  return EOS_TYPE_FLEXY_GRID_CELL;
}

static gboolean
eos_flexy_grid_motion_notify (GtkWidget *widget,
                              GdkEventMotion *event)
{
  EosFlexyGrid *self = EOS_FLEXY_GRID (widget);

  GdkWindow *window = gtk_widget_get_window (widget);
  GdkWindow *event_window = event->window;

  double relative_x = event->x;
  double relative_y = event->y;
  double parent_x, parent_y;

  while (event_window != NULL && event_window != window)
    {
      gdk_window_coords_to_parent (event_window,
                                   relative_x, relative_y,
                                   &parent_x, &parent_y);
      relative_x = parent_x;
      relative_y = parent_y;
      event_window = gdk_window_get_effective_parent (event_window);
    }

  EosFlexyGridCell *cell = eos_flexy_grid_get_cell_at_coords (self, relative_x, relative_y);
  if (cell != NULL)
    eos_flexy_grid_update_cell_prelight (self, cell);

  return GDK_EVENT_PROPAGATE;
}

static gboolean
eos_flexy_grid_button_press (GtkWidget *widget,
                             GdkEventButton *event)
{
  if (event->button != GDK_BUTTON_PRIMARY)
    return GDK_EVENT_PROPAGATE;

  EosFlexyGrid *self = EOS_FLEXY_GRID (widget);
  EosFlexyGridPrivate *priv = self->priv;

  priv->active_cell = NULL;

  GdkWindow *window = gtk_widget_get_window (widget);
  GdkWindow *event_window = event->window;

  double relative_x = event->x;
  double relative_y = event->y;
  double parent_x, parent_y;

  while (event_window != NULL && event_window != window)
    {
      gdk_window_coords_to_parent (event_window,
                                   relative_x, relative_y,
                                   &parent_x, &parent_y);
      relative_x = parent_x;
      relative_y = parent_y;
      event_window = gdk_window_get_effective_parent (event_window);
    }

  EosFlexyGridCell *cell = eos_flexy_grid_get_cell_at_coords (self, relative_x, relative_y);
  if (cell != NULL && gtk_widget_is_sensitive (GTK_WIDGET (cell)))
    {
      if (event->type == GDK_BUTTON_PRESS)
        {
          priv->active_cell = cell;
          gtk_widget_set_state_flags (GTK_WIDGET (priv->active_cell),
                                      GTK_STATE_FLAG_ACTIVE,
                                      FALSE);
          gtk_widget_queue_draw (widget);
        }
    }

  return GDK_EVENT_PROPAGATE;
}

static gboolean
eos_flexy_grid_button_release (GtkWidget *widget,
                               GdkEventButton *event)
{
  /* the widget may be destroyed in response to this event */
  g_object_ref (widget);

  if (event->button == GDK_BUTTON_PRIMARY)
    {
      EosFlexyGrid *grid = EOS_FLEXY_GRID (widget);
      EosFlexyGridPrivate *priv = grid->priv;

      if (priv->active_cell != NULL)
        {
          gtk_widget_unset_state_flags (GTK_WIDGET (priv->active_cell),
                                        GTK_STATE_FLAG_ACTIVE);
          g_signal_emit (grid, grid_signals[CELL_ACTIVATED], 0, priv->active_cell);
        }

      priv->active_cell = NULL;
      gtk_widget_queue_draw (widget);
    }

  g_object_unref (widget);

  return GDK_EVENT_PROPAGATE;
}

static gboolean
eos_flexy_grid_enter_notify (GtkWidget        *widget,
                             GdkEventCrossing *event)
{
  if (event->window != gtk_widget_get_window (widget))
    return GDK_EVENT_PROPAGATE;

  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (widget)->priv;

  priv->in_widget = TRUE;

  EosFlexyGridCell *cell = eos_flexy_grid_get_cell_at_coords (EOS_FLEXY_GRID (widget),
                                                              event->x,
                                                              event->y);
  eos_flexy_grid_update_cell_prelight (EOS_FLEXY_GRID (widget), cell);

  return GDK_EVENT_PROPAGATE;
}

static gboolean
eos_flexy_grid_leave_notify (GtkWidget        *widget,
                             GdkEventCrossing *event)
{
  if (event->window != gtk_widget_get_window (widget))
    return GDK_EVENT_PROPAGATE;

  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (widget)->priv;
  EosFlexyGridCell *cell;

  if (event->detail != GDK_NOTIFY_INFERIOR)
    {
      priv->in_widget = FALSE;
      cell = NULL;
    }
  else
    cell = eos_flexy_grid_get_cell_at_coords (EOS_FLEXY_GRID (widget),
                                              event->x,
                                              event->y);

  eos_flexy_grid_update_cell_prelight (EOS_FLEXY_GRID (widget), cell);

  return GDK_EVENT_PROPAGATE;
}

static void
eos_flexy_grid_finalize (GObject *gobject)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (gobject)->priv;

  if (priv->sort_notify != NULL)
    priv->sort_notify (priv->sort_data);

  g_clear_object (&priv->adjustment);

  g_sequence_free (priv->children);

  G_OBJECT_CLASS (eos_flexy_grid_parent_class)->finalize (gobject);
}

static void
eos_flexy_grid_class_init (EosFlexyGridClass *klass)
{
#if !GLIB_CHECK_VERSION (2, 37, 5)
  g_type_class_add_private (klass, sizeof (EosFlexyGridPrivate));
#endif

  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = eos_flexy_grid_finalize;

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  widget_class->get_request_mode = eos_flexy_grid_get_request_mode;
  widget_class->get_preferred_width = eos_flexy_grid_get_preferred_width;
  widget_class->get_preferred_height = eos_flexy_grid_get_preferred_height;
  widget_class->get_preferred_width_for_height = eos_flexy_grid_get_preferred_width_for_height;
  widget_class->get_preferred_height_for_width = eos_flexy_grid_get_preferred_height_for_width;
  widget_class->size_allocate = eos_flexy_grid_size_allocate;
  widget_class->draw = eos_flexy_grid_draw;
  widget_class->realize = eos_flexy_grid_realize;
  widget_class->motion_notify_event = eos_flexy_grid_motion_notify;
  widget_class->button_press_event = eos_flexy_grid_button_press;
  widget_class->button_release_event = eos_flexy_grid_button_release;
  widget_class->enter_notify_event = eos_flexy_grid_enter_notify;
  widget_class->leave_notify_event = eos_flexy_grid_leave_notify;

  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  container_class->add = eos_flexy_grid_add;
  container_class->remove = eos_flexy_grid_remove;
  container_class->forall = eos_flexy_grid_forall;
  container_class->child_type = eos_flexy_grid_child_type;

  grid_signals[CELL_SELECTED] =
    g_signal_new (g_intern_static_string ("cell-selected"),
                  EOS_TYPE_FLEXY_GRID,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (EosFlexyGridClass, cell_selected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  EOS_TYPE_FLEXY_GRID_CELL);

  grid_signals[CELL_ACTIVATED] =
    g_signal_new (g_intern_static_string ("cell-activated"),
                  EOS_TYPE_FLEXY_GRID,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (EosFlexyGridClass, cell_activated),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  EOS_TYPE_FLEXY_GRID_CELL);
}

static void
eos_flexy_grid_init (EosFlexyGrid *self)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID_GET_PRIV (self);

  priv->children = g_sequence_new (NULL);

  /* we use the same width as the discovery center layout */
  priv->cell_size = -1;
  priv->cell_spacing = DEFAULT_SPACING;

  /* XXX: once we depend on GTK 3.10 and GLib 2.38, we should remove this */
  self->priv = priv;

  GtkWidget *widget = GTK_WIDGET (self);
  gtk_widget_set_has_window (widget, TRUE);
  gtk_widget_set_redraw_on_allocate (widget, TRUE);

  GtkStyleContext *context = gtk_widget_get_style_context (widget);
  gtk_style_context_add_class (context, EOS_STYLE_CLASS_FLEXY_GRID);
}

GtkWidget *
eos_flexy_grid_new (void)
{
  return g_object_new (EOS_TYPE_FLEXY_GRID, NULL);
}

void
eos_flexy_grid_set_sort_func (EosFlexyGrid         *grid,
                              EosFlexyGridSortFunc  sort_func,
                              gpointer              data,
                              GDestroyNotify        notify)
{
  g_return_if_fail (EOS_IS_FLEXY_GRID (grid));

  EosFlexyGridPrivate *priv = grid->priv;
  if (priv->sort_notify != NULL)
    priv->sort_notify (priv->sort_data);

  priv->sort_func = sort_func;
  priv->sort_data = data;
  priv->sort_notify = notify;
}

void
eos_flexy_grid_set_cell_size (EosFlexyGrid *grid,
                              int           size)
{
  g_return_if_fail (EOS_IS_FLEXY_GRID (grid));

  EosFlexyGridPrivate *priv = grid->priv;
  if (priv->cell_size == size)
    return;

  priv->cell_size = size;

  gtk_widget_queue_resize (GTK_WIDGET (grid));
}

void
eos_flexy_grid_set_cell_spacing (EosFlexyGrid *grid,
                                 int           spacing)
{
  g_return_if_fail (EOS_IS_FLEXY_GRID (grid));

  EosFlexyGridPrivate *priv = grid->priv;
  if (priv->cell_spacing == spacing)
    return;

  priv->cell_spacing = spacing;

  gtk_widget_queue_resize (GTK_WIDGET (grid));
}

static gint
do_grid_sort (gconstpointer row_a,
              gconstpointer row_b,
              gpointer data)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (data)->priv;

  return priv->sort_func ((EosFlexyGridCell *) row_a,
                          (EosFlexyGridCell *) row_b,
                          priv->sort_data);
}

void
eos_flexy_grid_insert (EosFlexyGrid *grid,
                       GtkWidget    *child,
                       gint          index_)
{
  g_return_if_fail (EOS_IS_FLEXY_GRID (grid));
  g_return_if_fail (EOS_IS_FLEXY_GRID_CELL (child) || GTK_IS_WIDGET (child));

  EosFlexyGridCell *cell;

  if (EOS_IS_FLEXY_GRID_CELL (child))
    cell = EOS_FLEXY_GRID_CELL (child);
  else
    {
      cell = EOS_FLEXY_GRID_CELL (eos_flexy_grid_cell_new ());
      gtk_container_add (GTK_CONTAINER (cell), child);
      gtk_widget_show (GTK_WIDGET (cell));
    }

  EosFlexyGridPrivate *priv = grid->priv;
  GSequenceIter *iter;

  if (priv->sort_func != NULL)
    {
      iter = g_sequence_insert_sorted (priv->children, cell,
                                       do_grid_sort,
                                       grid);
    }
  else if (index_ == 0)
    iter = g_sequence_prepend (priv->children, cell);
  else if (index_ == -1)
    iter = g_sequence_append (priv->children, cell);
  else
    {
      GSequenceIter *cur_iter = g_sequence_get_iter_at_pos (priv->children, index_);

      iter = g_sequence_insert_before (cur_iter, cell);
    }

  eos_flexy_grid_cell_set_iter (cell, iter);

  gtk_widget_set_parent (GTK_WIDGET (cell), GTK_WIDGET (grid));
  gtk_widget_set_child_visible (GTK_WIDGET (cell), TRUE);
}
