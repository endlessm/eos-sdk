/* Copyright 2015-2016 Endless Mobile, Inc. */

#include <gtk/gtk.h>

#include "eoscellrenderertextlink-private.h"

#define LINK_NORMAL_FOREGROUND_COLOR "#3465a4"  /* sky blue 2 */
#define LINK_HOVER_FOREGROUND_COLOR "#729fcf"  /* sky blue 3 */

G_DEFINE_TYPE (EosCellRendererTextLink, eos_cell_renderer_text_link, GTK_TYPE_CELL_RENDERER_TEXT)

enum {
  CLICKED,
  LAST_SIGNAL
};

static guint text_link_signals[LAST_SIGNAL] = { 0 };

static void
eos_cell_renderer_text_link_render (GtkCellRenderer     *renderer,
                                    cairo_t             *cr,
                                    GtkWidget           *widget,
                                    const GdkRectangle  *background_area,
                                    const GdkRectangle  *cell_area,
                                    GtkCellRendererState flags)
{
  /* FIXME: the prelit flag is TRUE when the mouse is over the row that this
  renderer is in - even if the mouse is not over the renderer itself. */
  if (flags & GTK_CELL_RENDERER_PRELIT)
    g_object_set (renderer,
                  "foreground", LINK_HOVER_FOREGROUND_COLOR,
                  NULL);
  else
    g_object_set (renderer,
                  "foreground", LINK_NORMAL_FOREGROUND_COLOR,
                  NULL);
  GTK_CELL_RENDERER_CLASS (eos_cell_renderer_text_link_parent_class)->
    render (renderer, cr, widget, background_area, cell_area, flags);
}

static gboolean
eos_cell_renderer_text_link_activate (GtkCellRenderer     *renderer,
                                      GdkEvent            *event,
                                      GtkWidget           *widget,
                                      const gchar         *path,
                                      const GdkRectangle  *background_area,
                                      const GdkRectangle  *cell_area,
                                      GtkCellRendererState flags)
{
  g_signal_emit (renderer, text_link_signals[CLICKED], 0, path);
  return TRUE;
}

static void
eos_cell_renderer_text_link_class_init (EosCellRendererTextLinkClass *klass)
{
  GtkCellRendererClass *renderer_class = GTK_CELL_RENDERER_CLASS (klass);

  renderer_class->render = eos_cell_renderer_text_link_render;
  renderer_class->activate = eos_cell_renderer_text_link_activate;

  text_link_signals[CLICKED] =
    g_signal_new ("clicked", EOS_TYPE_CELL_RENDERER_TEXT_LINK,
                  G_SIGNAL_RUN_FIRST,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__STRING,
                  G_TYPE_NONE,
                  1, G_TYPE_STRING);
}

static void
eos_cell_renderer_text_link_init (EosCellRendererTextLink *self)
{
  g_object_set (self,
                "mode", GTK_CELL_RENDERER_MODE_ACTIVATABLE,
                NULL);
}

GtkCellRenderer *
eos_cell_renderer_text_link_new (void)
{
  return GTK_CELL_RENDERER (g_object_new (EOS_TYPE_CELL_RENDERER_TEXT_LINK,
                                          NULL));
}
