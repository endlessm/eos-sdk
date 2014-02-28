/* Copyright 2014 Endless Mobile, Inc. */

#include "config.h"
#include "eosobjectpreviewerutils.h"

#include <gtk/gtk.h>
#include <evince-document.h>

/**
 * eos_get_evince_supported_formats:
 *
 * Obtains the available information about the image formats supported
 * by GdkPixbuf. The evince function ev_backends_manager_get_all_types_info
 * so we need this to be in C.
 *
 * Returns: (element-type utf8) (transfer full): A list of
 * mime type strings supported by the evince document viewer.
 */
GList *
eos_get_evince_supported_formats (void)
{
  GList *iter, *formats = NULL;

  for (iter = ev_backends_manager_get_all_types_info (); iter; iter=iter->next) {
    EvTypeInfo *type_info = (EvTypeInfo *)iter->data;
    char **mime_types = type_info->mime_types;
    for (int i = 0; mime_types[i] != NULL; i++) {
      formats = g_list_prepend (formats, g_strdup (mime_types[i]));
    }
  }

  return formats;
}
