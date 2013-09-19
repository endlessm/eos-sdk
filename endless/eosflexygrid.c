/* Copyright 2013 Endless Mobile, Inc. */

/**
 * SECTION:eos-flexy-grid
 * @Title: EosFlexyGrid
 * @Short_Description: A flexible grid layout manager
 *
 * The #EosFlexyGrid widget provides a grid of cells in a layout controlled
 * by the shape of the cells themselves, through the #EosFlexyGridCell:shape
 * property of #EosFlexyGridCell.
 */

#include "config.h"

#include "eosflexygrid-private.h"

#include <string.h>
#include <glib-object.h>
#include <gtk/gtk.h>

/*#define VERBOSE 1*/

#define DEFAULT_CELL_SIZE       180
#define DEFAULT_SPACING         15

#ifdef VERBOSE
#define DEBUG(x)        x
#else
#define DEBUG(x)
#endif

typedef struct {
  GSequence *children;

  EosFlexyGridSortFunc sort_func;
  gpointer sort_data;
  GDestroyNotify sort_notify;

  int cell_size;
  int cell_spacing;

  EosFlexyGridCell *prelight_cell;
  EosFlexyGridCell *active_cell;

  guint in_widget : 1;
} EosFlexyGridPrivate;

enum
{
  PROP_0,

  PROP_CELL_SIZE,
  PROP_CELL_SPACING,

  LAST_PROP
};

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
static GParamSpec *grid_props[LAST_PROP] = { NULL, };

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

static inline void
add_new_empty_line (GArray *array,
                    guint   n_cols)
{
  guint start = array->len;

  g_array_set_size (array, array->len + n_cols);
  for (guint i = start; i < array->len; i++)
    g_array_index (array, gboolean, i) = TRUE;
}

static guint
get_next_free_slot (GArray *array,
                    guint   pos,
                    guint   n_cols)
{
  guint new_pos = pos;

  if (new_pos >= array->len)
    {
      DEBUG (g_print ("Adding empty line to cover for %u\n", pos));
      add_new_empty_line (array, n_cols);
    }

  while (g_array_index (array, gboolean, new_pos) == FALSE)
    {
      new_pos += 1;
      if (new_pos >= array->len)
        add_new_empty_line (array, n_cols);
    }

  DEBUG (g_print ("Next free slot after %u: %u\n", pos, new_pos));

  return new_pos;
}

static gboolean
check_free_slot (GArray *array,
                 guint   pos,
                 guint   n_cols)
{
  while (pos >= array->len)
    {
      DEBUG (g_print ("Adding empty line to cover for pos %u\n", pos));
      add_new_empty_line (array, n_cols);
    }

  DEBUG (g_print ("Slot %u is %s\n", pos, g_array_index (array, gboolean, pos) ? "free" : "occupied"));

  return g_array_index (array, gboolean, pos) == TRUE;
}

#define get_column(n_cols,pos)  ((pos) % (n_cols))
#define get_line(n_cols,pos)    ((pos) / (n_cols))

static inline void
set_position (GtkAllocation *request,
              guint          n_cols,
              guint          pos,
              guint          cell_width,
              guint          spacing)
{
  guint width = cell_width + spacing;

  request->y = get_line (n_cols, pos) * width;
  request->x = get_column (n_cols, pos) * width;
}

static inline void
mark_occupied_slot (GArray *array,
                    guint   pos)
{
  g_array_index (array, gboolean, pos) = FALSE;
}

static guint
allocate_small_cell (GArray        *array,
                     guint          n_cols,
                     guint          pos,
                     guint          cell_width,
                     guint          spacing,
                     GtkAllocation *request)
{
  request->width = cell_width;
  request->height = cell_width;

  set_position (request, n_cols, pos, cell_width, spacing);
  mark_occupied_slot (array, pos);

  DEBUG (g_print ("1-Cell[%u (column %u of %u, line %u)] = { %d, %d - %d x %d }, next: %u\n",
                  pos, get_column (n_cols, pos), n_cols, get_line (n_cols, pos),
                  request->x,
                  request->y,
                  request->width,
                  request->height,
                  get_next_free_slot (array, pos, n_cols)));

  return get_next_free_slot (array, pos, n_cols);
}

static guint
allocate_medium_cell (GArray         *array,
                      guint           n_cols,
                      guint           pos,
                      guint           cell_width,
                      guint           spacing,
                      GtkOrientation  orientation,
                      GtkAllocation  *request)
{
  guint check_pos = pos;
  guint check_column = get_column (n_cols, check_pos);

  switch (orientation)
    {
    case GTK_ORIENTATION_HORIZONTAL:
      request->width = 2 * cell_width + spacing;
      request->height = cell_width;

      /* two adjacent cells on the same line must be free */
      while (!(check_column < (n_cols - 1) && check_free_slot (array, check_pos + 1, n_cols)))
        {
          check_pos += 1;
          check_pos = get_next_free_slot (array, check_pos, n_cols);
          check_column = get_column (n_cols, check_pos);
        }

      set_position (request, n_cols, check_pos, cell_width, spacing);
      mark_occupied_slot (array, check_pos);
      mark_occupied_slot (array, check_pos + 1);
      break;

    case GTK_ORIENTATION_VERTICAL:
      request->width = cell_width;
      request->height = 2 * cell_width + spacing;

      /* two adjacent cells on the same column must be free */
      while (!check_free_slot (array, check_pos + n_cols, n_cols))
        {
          check_pos += 1;
          check_pos = get_next_free_slot (array, check_pos, n_cols);
          check_column = get_column (n_cols, check_pos);
        }

      set_position (request, n_cols, check_pos, cell_width, spacing);
      mark_occupied_slot (array, check_pos);
      mark_occupied_slot (array, check_pos + n_cols);
      break;
    }

  DEBUG (g_print ("2-Cell[%u (column %u of %u, line %u)] = { %d, %d - %d x %d }, next: %u\n",
                  pos, get_column (n_cols, pos), n_cols, get_line (n_cols, pos),
                  request->x,
                  request->y,
                  request->width,
                  request->height,
                  get_next_free_slot (array, pos, n_cols)));

  return get_next_free_slot (array, pos, n_cols);
}

static guint
allocate_large_cell (GArray        *array,
                     guint          n_cols,
                     guint          pos,
                     guint          cell_width,
                     guint          spacing,
                     GtkAllocation *request)
{
  request->width = 2 * cell_width + spacing;
  request->height = 2 * cell_width + spacing;

  guint check_pos = pos;
  guint check_column = get_column (n_cols, check_pos);

  while (!(check_column < (n_cols - 1) &&
           check_free_slot (array, check_pos + n_cols, n_cols) &&
           check_free_slot (array, check_pos + 1, n_cols) &&
           check_free_slot (array, check_pos + n_cols + 1, n_cols)))
    {
      check_pos += 1;
      check_pos = get_next_free_slot (array, check_pos, n_cols);
      check_column = get_column (n_cols, check_pos);
    }

  set_position (request, n_cols, check_pos, cell_width, spacing);
  mark_occupied_slot (array, check_pos);
  mark_occupied_slot (array, check_pos + 1);
  mark_occupied_slot (array, check_pos + n_cols);
  mark_occupied_slot (array, check_pos + n_cols + 1);

  DEBUG (g_print ("4-Cell[%u (column %u of %u, line %u)] = { %d, %d - %d x %d }, next: %u\n",
                  pos, get_column (n_cols, pos), n_cols, get_line (n_cols, pos),
                  request->x,
                  request->y,
                  request->width,
                  request->height,
                  get_next_free_slot (array, pos, n_cols)));

  return get_next_free_slot (array, pos, n_cols);
}

static int
distribute_layout (GSequence *children,
                   int        available_width,
                   int        cell_width,
                   int        spacing,
                   gboolean   allocate)
{
  guint n_columns = MAX (available_width / (cell_width + spacing), 2);
  guint real_width = cell_width;
  GArray *array = g_array_new (FALSE, FALSE, sizeof (gboolean));
  guint current_pos = 0;
  int max_height = cell_width;

  add_new_empty_line (array, n_columns);

  GSequenceIter *iter;
  for (iter = g_sequence_get_begin_iter (children);
       !g_sequence_iter_is_end (iter);
       iter = g_sequence_iter_next (iter))
    {
      EosFlexyGridCell *cell = g_sequence_get (iter);
      EosFlexyShape shape = eos_flexy_grid_cell_get_shape (cell);
      GtkAllocation request = { 0, };

      switch (shape)
        {
        case EOS_FLEXY_SHAPE_SMALL:
          current_pos = allocate_small_cell (array,
                                             n_columns, current_pos,
                                             real_width, spacing,
                                             &request);
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_HORIZONTAL:
          current_pos = allocate_medium_cell (array,
                                              n_columns, current_pos,
                                              real_width, spacing,
                                              GTK_ORIENTATION_HORIZONTAL,
                                              &request);
          break;

        case EOS_FLEXY_SHAPE_MEDIUM_VERTICAL:
          current_pos = allocate_medium_cell (array,
                                              n_columns, current_pos,
                                              real_width, spacing,
                                              GTK_ORIENTATION_VERTICAL,
                                              &request);
          break;

        case EOS_FLEXY_SHAPE_LARGE:
          current_pos = allocate_large_cell (array,
                                             n_columns, current_pos,
                                             real_width, spacing,
                                             &request);
          break;
        }

      max_height = MAX (max_height, request.y + request.height + spacing);

      if (allocate)
        gtk_widget_size_allocate (GTK_WIDGET (cell), &request);
    }

  g_array_unref (array);

  DEBUG (g_print ("%s size: { %d x %d }\n",
                  allocate ? "Allocated" : "Preferred",
                  available_width,
                  max_height));

  return max_height;
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

  int cell_size = priv->cell_size < 0 ? DEFAULT_CELL_SIZE : priv->cell_size;
  int cell_spacing = priv->cell_spacing < 0 ? DEFAULT_SPACING : priv->cell_spacing;
  int layout_height;

  layout_height = distribute_layout (priv->children, for_width, cell_size, cell_spacing, FALSE);

  if (minimum_height_out)
    *minimum_height_out = layout_height;

  if (natural_height_out)
    *natural_height_out = layout_height;
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

  int cell_size = priv->cell_size < 0 ? DEFAULT_CELL_SIZE : priv->cell_size;
  int cell_spacing = priv->cell_spacing < 0 ? DEFAULT_SPACING : priv->cell_spacing;
  int available_width = allocation->width;

  distribute_layout (priv->children, available_width, cell_size, cell_spacing, TRUE);
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
eos_flexy_grid_set_property (GObject      *gobject,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  EosFlexyGrid *self = EOS_FLEXY_GRID (gobject);

  switch (prop_id)
    {
    case PROP_CELL_SIZE:
      eos_flexy_grid_set_cell_size (self, g_value_get_int (value));
      break;

    case PROP_CELL_SPACING:
      eos_flexy_grid_set_cell_spacing (self, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
eos_flexy_grid_get_property (GObject    *gobject,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  EosFlexyGrid *self = EOS_FLEXY_GRID (gobject);

  switch (prop_id)
    {
    case PROP_CELL_SIZE:
      g_value_set_int (value, eos_flexy_grid_get_cell_size (self));
      break;

    case PROP_CELL_SPACING:
      g_value_set_int (value, eos_flexy_grid_get_cell_spacing (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
    }
}

static void
eos_flexy_grid_finalize (GObject *gobject)
{
  EosFlexyGridPrivate *priv = EOS_FLEXY_GRID (gobject)->priv;

  if (priv->sort_notify != NULL)
    priv->sort_notify (priv->sort_data);

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
  gobject_class->set_property = eos_flexy_grid_set_property;
  gobject_class->get_property = eos_flexy_grid_get_property;

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

  /**
   * EosFlexyGrid:cell-size:
   *
   * The minimum size of each cell inside a #EosFlexyGrid, or -1 for the default.
   */
  grid_props[PROP_CELL_SIZE] =
    g_param_spec_int ("cell-size",
                      "Cell Size",
                      "The minimum size of each cell",
                      -1, G_MAXINT,
                      -1,
                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosFlexyGrid:cell-spacing:
   *
   * The spacing between each cell inside a #EosFlexyGrid, or -1 for the default.
   */
  grid_props[PROP_CELL_SPACING] =
    g_param_spec_int ("cell-spacing",
                      "Cell Spacing",
                      "The spacing between each cell",
                      -1, G_MAXINT,
                      -1,
                      G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class, LAST_PROP, grid_props);

  /**
   * EosFlexyGrid::cell-selected:
   * @grid: the #EosFlexyGrid that emitted the signal
   * @cell: the #EosFlexyGridCell that was selected
   *
   * The ::cell-selected signal is emitted each time a cell inside @grid
   * is selected. Selection happens by hovering on a cell.
   */
  grid_signals[CELL_SELECTED] =
    g_signal_new (g_intern_static_string ("cell-selected"),
                  EOS_TYPE_FLEXY_GRID,
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (EosFlexyGridClass, cell_selected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__OBJECT,
                  G_TYPE_NONE, 1,
                  EOS_TYPE_FLEXY_GRID_CELL);

  /**
   * EosFlexyGrid::cell-activated:
   * @grid: the #EosFlexyGrid that emitted the signal
   * @cell: the #EosFlexyGridCell that was activated
   *
   * The ::cell-activated signal is emitted each time a cell inside @grid
   * is activated. Activation happens by clicking on a cell.
   */
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

/**
 * eos_flexy_grid_new:
 *
 * Creates a new #EosFlexyGrid widget.
 *
 * Return value: (transfer full): the newly created #EosFlexyGrid widget
 */
GtkWidget *
eos_flexy_grid_new (void)
{
  return g_object_new (EOS_TYPE_FLEXY_GRID, NULL);
}

/**
 * eos_flexy_grid_set_sort_func:
 * @grid: a #EosFlexyGrid
 * @sort_func: (scope notified) (allow-none): a sorting function, or
 *   %NULL to unset an existing one
 * @data: (closure): data to pass to @sort_func and @notify
 * @notify: function called when @sort_func is unset, or the @grid
 *   widget is destroyed
 *
 * Sets the sorting function for @grid.
 *
 * The @sort_func function compares two children of @grid, and must
 * return -1 if the first child should precede the second; 1, if the
 * first child should follow the second; or 0, if the children are
 * identical.
 *
 * The @notify function will be called when this function is called
 * with a different @sort_func (or %NULL); or when the @grid widget
 * is destroyed.
 */
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

/**
 * eos_flexy_grid_set_cell_size:
 * @grid: a #EosFlexyGrid
 * @size: the size of the cell
 *
 * Sets the size of the cells of @grid.
 *
 * If @size is less than 0, the default size will be used.
 */
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

/**
 * eos_flexy_grid_get_cell_size:
 * @grid: a #EosFlexyGrid
 *
 * Retrieves the size of the cells of @grid.
 *
 * Return value: the size of the cells
 */
guint
eos_flexy_grid_get_cell_size (EosFlexyGrid *grid)
{
  g_return_val_if_fail (EOS_IS_FLEXY_GRID (grid), 0);

  EosFlexyGridPrivate *priv = grid->priv;

  if (priv->cell_size < 0)
    return DEFAULT_CELL_SIZE;

  return priv->cell_size;
}

/**
 * eos_flexy_grid_set_cell_spacing:
 * @grid: a #EosFlexyGrid
 * @spacing: the spacing between each cell
 *
 * Sets the spacing between each cell of @grid.
 *
 * If @spacing is less than 0, the default value will be used.
 */
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

/**
 * eos_flexy_grid_get_cell_spacing:
 * @grid: a #EosFlexyGrid
 *
 * Retrieves the cell spacing of @grid.
 *
 * Return value: the spacing between each cell
 */
guint
eos_flexy_grid_get_cell_spacing (EosFlexyGrid *grid)
{
  g_return_val_if_fail (EOS_IS_FLEXY_GRID (grid), 0);

  EosFlexyGridPrivate *priv = grid->priv;

  if (priv->cell_spacing < 0)
    return DEFAULT_SPACING;

  return priv->cell_spacing;
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

/**
 * eos_flexy_grid_insert:
 * @grid: a #EosFlexyGrid
 * @child: a #GtkWidget
 * @index_: the position of the @child
 *
 * Inserts @child inside @grid, at the given @index_. If @child is not
 * a #EosFlexyGridCell widget, one will be implicitly created, and @child
 * added to it.
 *
 * If @grid has a sort function, the @index_ is ignored.
 *
 * If @index_ is less than 0, the @child is appended at the end of the grid.
 *
 * If @index_ is 0, the child is prepended at the beginning of the grid.
 */
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

/**
 * eos_flexy_grid_get_cell_at_coords:
 * @grid: a #EosFlexyGrid
 * @x_pos: X coordinate to test, in widget-relative space
 * @y_pos: Y coordinate to test, in widget-relative space
 *
 * Retrieves the #EosFlexyGridCell at the given coordinates.
 *
 * The coordinates to test must be in widget-relative space.
 *
 * Return value: (transfer none): the cell at the given coordinates, or %NULL
 */
EosFlexyGridCell *
eos_flexy_grid_get_cell_at_coords (EosFlexyGrid *grid,
                                   double        x_pos,
                                   double        y_pos)
{
  g_return_val_if_fail (EOS_IS_FLEXY_GRID (grid), NULL);

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

