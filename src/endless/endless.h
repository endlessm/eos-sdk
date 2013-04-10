#ifndef ENDLESS_H
#define ENDLESS_H

#include <glib.h>
#include <gio/gio.h>

G_BEGIN_DECLS

gboolean eos_hello_sample_function (GFile   *file,
                                    GError **error);

G_END_DECLS

#endif

