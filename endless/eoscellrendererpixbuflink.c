/* Copyright 2015 Endless Mobile, Inc. */

#include <gtk/gtk.h>

#include "eoscellrendererpixbuflink-private.h"

G_DEFINE_TYPE (EosCellRendererPixbufLink, eos_cell_renderer_pixbuf_link, GTK_TYPE_CELL_RENDERER_PIXBUF)

enum {
  CLICKED,
  LAST_SIGNAL
};

static guint pixbuf_link_signals[LAST_SIGNAL] = { 0 };

static gboolean
eos_cell_renderer_pixbuf_link_activate (GtkCellRenderer     *renderer,
                                        GdkEvent            *event,
                                        GtkWidget           *widget,
                                        const gchar         *path,
                                        const GdkRectangle  *background_area,
                                        const GdkRectangle  *cell_area,
                                        GtkCellRendererState flags)
{
  g_signal_emit (renderer, pixbuf_link_signals[CLICKED], 0, path);
  return TRUE;
}

static void
eos_cell_renderer_pixbuf_link_class_init (EosCellRendererPixbufLinkClass *klass)
{
  GtkCellRendererClass *renderer_class = GTK_CELL_RENDERER_CLASS (klass);

  renderer_class->activate = eos_cell_renderer_pixbuf_link_activate;

  pixbuf_link_signals[CLICKED] =
    g_signal_new ("clicked", EOS_TYPE_CELL_RENDERER_PIXBUF_LINK,
                  G_SIGNAL_RUN_FIRST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE,
                  1, G_TYPE_STRING);
}

static void
eos_cell_renderer_pixbuf_link_init (EosCellRendererPixbufLink *self)
{
  g_object_set (self,
                "mode", GTK_CELL_RENDERER_MODE_ACTIVATABLE,
                NULL);
}

GtkCellRenderer *
eos_cell_renderer_pixbuf_link_new (void)
{
  return GTK_CELL_RENDERER (g_object_new (EOS_TYPE_CELL_RENDERER_PIXBUF_LINK,
                                          NULL));
}
