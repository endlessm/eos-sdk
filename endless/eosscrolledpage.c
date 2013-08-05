/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eosscrolledpage-private.h"

#include <glib-object.h>
#include <gtk/gtk.h>
#include <math.h>

#define _EOS_STYLE_CLASS_SCROLLED_PAGE "scrolled-page"

#define DEFAULT_ANIMATION_DURATION 1000000 // 1 second

/*
 * SECTION:scrolled-page
 * @short_description: A page that scrolls to reveal and hide its children
 * @title: Scrolled Page
 */

G_DEFINE_TYPE (EosScrolledPage, eos_scrolled_page, GTK_TYPE_SCROLLED_WINDOW)

#define EOS_SCROLLED_PAGE_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_SCROLLED_PAGE, EosScrolledPagePrivate))

struct _EosScrolledPagePrivate
{
  GtkWidget *grid;
  GtkWidget *edge_widget;
  gint64 animation_start;
  gdouble animation_duration;
  gdouble animation_origin;
  gdouble animation_target;
};

enum {
  PROP_0,
  PROP_EDGE_WIDGET
};

static void eos_scrolled_page_dispose (GObject *object);

static void eos_scrolled_page_finalize (GObject *object);

static void eos_scrolled_page_size_allocate (GtkWidget        *widget,
                                             GtkAllocation    *allocation);
static void eos_scrolled_page_get_property (GObject    *object,
                                            guint       property_id,
                                            GValue     *value,
                                            GParamSpec *pspec);

static void eos_scrolled_page_set_property (GObject      *object,
                                            guint         property_id,
                                            const GValue *value,
                                            GParamSpec   *pspec);

static void animate_scroll (EosScrolledPage *page, GtkWidget* edge_widget);

static void on_add_widget (GtkContainer *container,
                           GtkWidget    *widget,
                           gpointer      user_data);

static void on_remove_widget (GtkContainer *container,
                              GtkWidget    *widget,
                              gpointer      user_data);

/* ******* INIT ******* */

static void
eos_scrolled_page_class_init (EosScrolledPageClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosScrolledPagePrivate));

  object_class->dispose = eos_scrolled_page_dispose;
  object_class->finalize = eos_scrolled_page_finalize;

  object_class->get_property = eos_scrolled_page_get_property;
  object_class->set_property = eos_scrolled_page_set_property;

  widget_class->size_allocate = eos_scrolled_page_size_allocate;

  g_object_class_install_property (object_class,
                                   PROP_EDGE_WIDGET,
                                   g_param_spec_object ("edge-widget",
                                                        "Edge widget",
                                                        "The widget at the edge of the scrolled page",
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READWRITE));
}

static void
eos_scrolled_page_init (EosScrolledPage *self)
{
  EosScrolledPagePrivate *priv;
  GtkStyleContext *context;

  self->priv = EOS_SCROLLED_PAGE_PRIVATE (self);
  priv = self->priv;

  context = gtk_widget_get_style_context (GTK_WIDGET (self));
  gtk_style_context_add_class (context, _EOS_STYLE_CLASS_SCROLLED_PAGE);

  g_object_set (G_OBJECT (self),
                "hexpand", TRUE,
                "vexpand", TRUE,
                "halign", GTK_ALIGN_FILL,
                "valign", GTK_ALIGN_FILL, NULL);

  priv->grid = gtk_grid_new ();

  g_object_set (G_OBJECT (priv->grid),
                "hexpand", TRUE,
                "vexpand", TRUE,
                "halign", GTK_ALIGN_FILL,
                "valign", GTK_ALIGN_FILL, NULL);

  gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (priv->grid));

  g_object_connect (G_OBJECT (priv->grid), "add", on_add_widget, self);
  g_object_connect (G_OBJECT (priv->grid), "remove", on_remove_widget, self);
}

/* ******* LIFECYCLE ******* */

/*
 * eos_scrolled_page_new:
 *
 * Returns: a new instance
 */
GtkWidget *
eos_scrolled_page_new ()
{
  return g_object_new (EOS_TYPE_SCROLLED_PAGE, NULL);
}

static void
eos_scrolled_page_dispose (GObject *object)
{
  G_OBJECT_CLASS (eos_scrolled_page_parent_class)->dispose (object);
}

static void
eos_scrolled_page_finalize (GObject *object)
{
  G_OBJECT_CLASS (eos_scrolled_page_parent_class)->finalize (object);
}

/* ******* PROPERTIES ******* */

void
eos_scrolled_page_set_edge_widget  (EosScrolledPage *self,
                                    GtkWidget *edge_widget)
{
  EosScrolledPagePrivate *priv;

  g_return_if_fail (EOS_IS_SCROLLED_PAGE (self));
  priv = self->priv;

  // IF container actually contains the widget

  if (edge_widget != priv->edge_widget)
    {
      priv->edge_widget = edge_widget;
      animate_scroll (self, edge_widget);
    }
}

GtkWidget*
eos_scrolled_page_get_edge_widget  (EosScrolledPage *self)
{
  EosScrolledPagePrivate *priv;

  g_return_val_if_fail (EOS_IS_SCROLLED_PAGE (self), NULL);
  priv = self->priv;

  return priv->edge_widget;
}

static void
eos_scrolled_page_get_property (GObject    *object,
                                guint       property_id,
                                GValue     *value,
                                GParamSpec *pspec)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (object);
  EosScrolledPagePrivate *priv = self->priv;

  switch (property_id)
  {
  case PROP_EDGE_WIDGET:
    g_value_set_object (value, priv->edge_widget);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
eos_scrolled_page_set_property (GObject      *object,
                                guint         property_id,
                                const GValue *value,
                                GParamSpec   *pspec)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (object);

  switch (property_id)
  {
  case PROP_EDGE_WIDGET :
    eos_scrolled_page_set_edge_widget (self, GTK_WIDGET (g_value_get_object (value)));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}


/* ******* LAYOUT ******* */

static void
recalculate_width_request (EosScrolledPage *self)
{
  EosScrolledPagePrivate *priv;

  g_return_if_fail (EOS_IS_SCROLLED_PAGE (self));
  priv = self->priv;

  // sets the size request of the inner grid according to its contents and
  // the space allocated to the scrolled window
  // it needs to make sure that the edge widget remains in the right place
}

static void
eos_scrolled_page_size_allocate (GtkWidget        *widget,
                                 GtkAllocation    *allocation)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (widget);

  // TODO : recalculate the size and allocation
  // set the width request to the grid, according to the new allocation
  // and, hopefully, doing this before calling size_allocate() will propagate
  // the changes correctly

  recalculate_width_request(self);

  GTK_WIDGET_CLASS (eos_scrolled_page_parent_class)->size_allocate (widget, allocation);
}

static void
on_add_widget (GtkContainer *container,
               GtkWidget    *widget,
               gpointer      user_data)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (widget);

  recalculate_width_request (self);
}

static void
on_remove_widget (GtkContainer *container,
                  GtkWidget    *widget,
                  gpointer      user_data)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (widget);

  recalculate_width_request (self);
}

/* ******* ANIMATION ******* */

static gboolean
on_tick_scroll_cb (GtkWidget *widget,
                   GdkFrameClock *frame_clock,
                   gpointer user_data)
{
  EosScrolledPage *self = EOS_SCROLLED_PAGE (widget);
  EosScrolledPagePrivate *priv = self->priv;
  GtkAdjustment *hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (self));

  gint64 now = gdk_frame_clock_get_frame_time (frame_clock) - priv->animation_start;
  gdouble completion = MIN (now/priv->animation_duration, 1.0);

  gtk_adjustment_set_value (hadj,
                            priv->animation_origin + (priv->animation_target - priv->animation_origin)*completion);

  return (now < priv->animation_duration);
}

static void
animate_scroll (EosScrolledPage *self, GtkWidget *edge_widget)
{
  EosScrolledPagePrivate *priv = self->priv;
  GtkAdjustment *hadj = gtk_scrollable_get_hadjustment (GTK_SCROLLABLE (self));

  if (!gtk_widget_get_realized (GTK_WIDGET (self)))
    {
      // do not try  to animate unrealized widgets
      return;
    }

  if (edge_widget)
    {
      GtkAllocation alloc;
      gtk_widget_get_allocation (edge_widget, &alloc);
      // upper - page_size is the maximum value for the scroll, otherwise we run out of widget
      priv->animation_target = MIN (alloc.x,
                                    gtk_adjustment_get_upper (hadj) - gtk_adjustment_get_page_size (hadj));
    }
  else
    {
      // if target is NULL, we scroll the page back to the beginning
      priv->animation_target = 0;
    }

  if (priv->animation_target != gtk_adjustment_get_value (hadj))
    {
      priv->animation_start = gdk_frame_clock_get_frame_time (gtk_widget_get_frame_clock (GTK_WIDGET (self)));
      priv->animation_origin = gtk_adjustment_get_value (hadj);
      gtk_widget_add_tick_callback (GTK_WIDGET (self),
                                    (GtkTickCallback) on_tick_scroll_cb,
                                    NULL, NULL);
    }
}
