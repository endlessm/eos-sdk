/* Copyright 2015-2016 Endless Mobile, Inc. */

#ifndef EOS_CELL_RENDERER_PIXBUF_LINK_H
#define EOS_CELL_RENDERER_PIXBUF_LINK_H

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_CELL_RENDERER_PIXBUF_LINK eos_cell_renderer_pixbuf_link_get_type()

#define EOS_CELL_RENDERER_PIXBUF_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_CELL_RENDERER_PIXBUF_LINK, EosCellRendererPixbufLink))

#define EOS_CELL_RENDERER_PIXBUF_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_CELL_RENDERER_PIXBUF_LINK, EosCellRendererPixbufLinkClass))

#define EOS_IS_CELL_RENDERER_PIXBUF_LINK(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_CELL_RENDERER_PIXBUF_LINK))

#define EOS_IS_CELL_RENDERER_PIXBUF_LINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_CELL_RENDERER_PIXBUF_LINK))

#define EOS_CELL_RENDERER_PIXBUF_LINK_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_CELL_RENDERER_PIXBUF_LINK, EosCellRendererPixbufLinkClass))

typedef struct _EosCellRendererPixbufLink EosCellRendererPixbufLink;
typedef struct _EosCellRendererPixbufLinkClass EosCellRendererPixbufLinkClass;

struct _EosCellRendererPixbufLink
{
  GtkCellRendererPixbuf parent;
};

struct _EosCellRendererPixbufLinkClass
{
  GtkCellRendererPixbufClass parent_class;
};

GType            eos_cell_renderer_pixbuf_link_get_type (void) G_GNUC_CONST;

GtkCellRenderer *eos_cell_renderer_pixbuf_link_new      (void);

G_END_DECLS

#endif /* EOS_CELL_RENDERER_PIXBUF_LINK_H */
