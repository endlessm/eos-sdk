/* Copyright 2015 Endless Mobile, Inc. */

#ifndef EOS_ATTRIBUTION_H
#define EOS_ATTRIBUTION_H

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_ATTRIBUTION eos_attribution_get_type()

#define EOS_ATTRIBUTION(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_ATTRIBUTION, EosAttribution))

#define EOS_ATTRIBUTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_ATTRIBUTION, EosAttributionClass))

#define EOS_IS_ATTRIBUTION(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_ATTRIBUTION))

#define EOS_IS_ATTRIBUTION_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_ATTRIBUTION))

#define EOS_ATTRIBUTION_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_ATTRIBUTION, EosAttributionClass))

typedef struct _EosAttribution EosAttribution;
typedef struct _EosAttributionClass EosAttributionClass;

struct _EosAttribution
{
  GtkScrolledWindow parent;
};

struct _EosAttributionClass
{
  GtkScrolledWindowClass parent_class;
};

GType      eos_attribution_get_type   (void) G_GNUC_CONST;

GtkWidget *eos_attribution_new_sync   (GFile              *file,
                                       GCancellable       *cancellable,
                                       GError            **error);

GFile *    eos_attribution_get_file   (EosAttribution     *self);

G_END_DECLS

#endif /* EOS_ATTRIBUTION_H */
