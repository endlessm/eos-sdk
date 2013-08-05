/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_SCROLLED_PAGE_H
#define EOS_SCROLLED_PAGE_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_SCROLLED_PAGE eos_scrolled_page_get_type()

#define EOS_SCROLLED_PAGE(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                 EOS_TYPE_SCROLLED_PAGE, EosScrolledPage))

#define EOS_SCROLLED_PAGE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST ((klass), \
                              EOS_TYPE_SCROLLED_PAGE, EosScrolledPageClass))

#define EOS_IS_SCROLLED_PAGE(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                 EOS_TYPE_SCROLLED_PAGE))

#define EOS_IS_SCROLLED_PAGE_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                              EOS_TYPE_SCROLLED_PAGE))

#define EOS_SCROLLED_PAGE_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                EOS_TYPE_SCROLLED_PAGE, EosScrolledPageClass))

typedef struct _EosScrolledPage EosScrolledPage;
typedef struct _EosScrolledPageClass EosScrolledPageClass;
typedef struct _EosScrolledPagePrivate EosScrolledPagePrivate;

struct _EosScrolledPage
{
  GtkScrolledWindow parent;

  EosScrolledPagePrivate *priv;
};

struct _EosScrolledPageClass
{
  GtkScrolledWindowClass parent_class;
};

GType        eos_scrolled_page_get_type              (void) G_GNUC_CONST;

GtkWidget   *eos_scrolled_page_new                   ();

GtkWidget   *eos_scrolled_page_get_edge_widget       (EosScrolledPage *self);

void         eos_scrolled_page_set_edge_widget       (EosScrolledPage *self,
                                                      GtkWidget *edge_widget);

G_END_DECLS

#endif /* EOS_SCROLLED_PAGE_H */
