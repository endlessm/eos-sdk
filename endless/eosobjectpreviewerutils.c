/* Copyright 2014 Endless Mobile, Inc. */

#include "config.h"
#include "eosobjectpreviewerutils.h"

#include <gtk/gtk.h>
#include <evince-document.h>
#include <clutter-gst/clutter-gst.h>

/**
 * eos_get_clutter_gst_supported_formats:
 *
 * TODO
 *
 * Returns: (element-type utf8) (transfer full) (allow none): A list of
 * mime type strings supported by ClutterGstVideoTexure.
 */

GList *
eos_get_clutter_gst_supported_formats (void)
{
  GList *formats = NULL;

  // FIXME!!
  GList* factoryList = gst_registry_get_feature_list(gst_registry_get (), GST_TYPE_ELEMENT_FACTORY);
  for (GList* iter = g_list_first(factoryList) ; iter != NULL ; iter = g_list_next(iter)) {
    GstPluginFeature *feature = GST_PLUGIN_FEATURE(iter->data);
    const GList *static_templates;
    GstElementFactory *factory = GST_ELEMENT_FACTORY(feature);
    static_templates = gst_element_factory_get_static_pad_templates(factory);
 
    for (; static_templates != NULL ; static_templates = static_templates->next) {
      GstStaticPadTemplate *pad_template = (GstStaticPadTemplate *) static_templates->data;
      if (pad_template && pad_template->direction == GST_PAD_SINK) {
        GstCaps *caps = gst_static_pad_template_get_caps (pad_template);
        if (caps) {
          for (unsigned int struct_idx = 0; struct_idx < gst_caps_get_size (caps); struct_idx++) {
            const GstStructure* capsStruct = gst_caps_get_structure (caps, struct_idx);
            gchar *mime = gst_structure_get_name (capsStruct);
            printf("%s\n", mime);
            if (g_strstr_len (mime, -1, "video") && !g_list_find_custom (formats, mime, g_strcmp0))
              formats = g_list_prepend (formats, g_strdup (mime));
          }
        }
      }
    }
  }

  return formats;
}

/**
 * eos_get_evince_supported_formats:
 *
 * Obtains the available information about the image formats supported
 * by GdkPixbuf. The evince function ev_backends_manager_get_all_types_info
 * so we need this to be in C.
 *
 * Returns: (element-type utf8) (transfer full) (allow none): A list of
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
