/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#include <string.h>

#include <glib.h>
#include <webkit2/webkit-web-extension.h>
#include <webkitdom/webkitdom.h>

#define WH2_DBUS_INTERFACE_NAME "com.endlessm.WebHelper2.Translation"
#define MAIN_PROGRAM_OBJECT_PATH "/com/endlessm/gettext"
#define MAIN_PROGRAM_INTERFACE_NAME "com.endlessm.WebHelper2.Gettext"

/* Declaration of externally visible symbol */
void webkit_web_extension_initialize_with_user_data (WebKitWebExtension *, const GVariant *);

typedef struct {
  GDBusConnection *connection;  /* unowned */
  GDBusNodeInfo *node;  /* owned */
  GDBusInterfaceInfo *interface;  /* owned by node */
  GSList *bus_ids;  /* GSList<guint>; owned */
  GSList *page_ctxts;  /* GSList<PageContext *>; owned */
  gchar *main_program_name;  /* owned; well-known-name of main program */
} Context;

typedef struct {
  Context *ctxt;  /* unowned */
  WebKitWebPage *page;  /* unowned */
} PageContext;

static const gchar introspection_xml[] =
  "<node>"
    "<interface name='" WH2_DBUS_INTERFACE_NAME "'>"
      "<method name='Translate'/>"
    "</interface>"
  "</node>";

static void
context_free (Context *ctxt)
{
  g_clear_pointer (&ctxt->node, g_dbus_node_info_unref);
  g_clear_pointer (&ctxt->bus_ids, g_slist_free);
  g_clear_pointer (&ctxt->main_program_name, g_free);
  g_slist_free_full (ctxt->page_ctxts, (GDestroyNotify) g_free);
  ctxt->page_ctxts = NULL;
  g_free (ctxt);
}

static gchar *
translation_function (const gchar *message,
                      Context     *ctxt)
{
  GError *error = NULL;
  GVariant *result =
    g_dbus_connection_call_sync (ctxt->connection, ctxt->main_program_name,
                                 MAIN_PROGRAM_OBJECT_PATH,
                                 MAIN_PROGRAM_INTERFACE_NAME, "Gettext",
                                 g_variant_new ("(s)", message),
                                 (GVariantType *) "(s)",
                                 G_DBUS_CALL_FLAGS_NO_AUTO_START,
                                 -1 /* timeout */, NULL /* cancellable */,
                                 &error);
  if (result == NULL)
    {
      g_warning ("No return value from gettext: %s", error->message);
      g_clear_error (&error);
      return g_strdup (message);
    }

  gchar *retval;
  g_variant_get (result, "(s)", &retval);
  g_variant_unref (result);
  return retval;
}

static void
translate_html (WebKitDOMDocument *dom,
                Context           *ctxt)
{
  WebKitDOMNodeList *translatable;
  GError *error = NULL;

  translatable = webkit_dom_document_get_elements_by_name (dom, "translatable");

  gulong index, length = webkit_dom_node_list_get_length (translatable);
  for (index = 0; index < length; index++)
    {
      WebKitDOMNode *element = webkit_dom_node_list_item (translatable, index);

      /* Translate the text */
      if (WEBKIT_DOM_IS_HTML_ELEMENT (element))
        {
          WebKitDOMHTMLElement *el_html = WEBKIT_DOM_HTML_ELEMENT (element);
          gchar *inner_html = webkit_dom_html_element_get_inner_html (el_html);
          gchar *translated_html = translation_function (inner_html, ctxt);
          webkit_dom_html_element_set_inner_html (el_html, translated_html,
                                                  &error);
          if (error != NULL)
            {
              g_warning ("There was a problem translating '%s' to '%s': %s",
                         inner_html, translated_html, error->message);
              g_clear_error (&error);
            }

          g_free (translated_html);
          g_free (inner_html);
        }
      else
        {
          gchar *text = webkit_dom_node_get_text_content (element);
          gchar *translated_text = translation_function (text, ctxt);
          webkit_dom_node_set_text_content (element, translated_text, &error);
          if (error != NULL)
            {
              g_warning ("There was a problem translating '%s' to '%s': %s",
                         text, translated_text, error->message);
              g_clear_error (&error);
            }

          g_free (translated_text);
          g_free (text);
        }
    }

  g_object_unref (translatable);
}

static void
on_wh2_method_call (GDBusConnection       *connection,
                    const gchar           *sender,
                    const gchar           *object_path,
                    const gchar           *interface_name,
                    const gchar           *method_name,
                    GVariant              *parameters,
                    GDBusMethodInvocation *invocation,
                    PageContext           *pctxt)
{
  if (strcmp (method_name, "Translate") != 0)
    {
      g_dbus_method_invocation_return_error (invocation, G_DBUS_ERROR,
                                             G_DBUS_ERROR_UNKNOWN_METHOD,
                                             "Unknown method %s invoked on interface %s",
                                             method_name, interface_name);
      return;
    }

  WebKitDOMDocument *document = webkit_web_page_get_dom_document (pctxt->page);
  if (document == NULL)
    {
      g_dbus_method_invocation_return_error_literal (invocation, G_IO_ERROR,
                                                     G_IO_ERROR_NOT_INITIALIZED,
                                                     "The web page has not loaded a document yet");
      return;
    }

  translate_html (document, pctxt->ctxt);

  g_dbus_method_invocation_return_value (invocation, NULL);
}

static GDBusInterfaceVTable dbus_impl_vtable = {
  (GDBusInterfaceMethodCallFunc) on_wh2_method_call,
  NULL, /* get property */
  NULL /* set property */
};

static gboolean
register_object (PageContext *pctxt)
{
  GError *error = NULL;

  if (pctxt->ctxt->connection == NULL)
    return G_SOURCE_CONTINUE;  /* Try again when the connection is ready */

  /* The ID is known to the main process and the web process. So we can address
  a specific web page over DBus. */
  guint64 id = webkit_web_page_get_id (pctxt->page);

  gchar *object_path = g_strdup_printf("/com/endlessm/webview/%"
                                       G_GUINT64_FORMAT, id);

  guint bus_id = g_dbus_connection_register_object (pctxt->ctxt->connection,
                                                    object_path,
                                                    pctxt->ctxt->interface,
                                                    &dbus_impl_vtable,
                                                    pctxt, NULL,
                                                    &error);
  if (bus_id == 0)
    {
      g_critical ("Failed to export webview object on bus: %s", error->message);
      g_clear_error (&error);
      goto out;
    }

  pctxt->ctxt->bus_ids = g_slist_prepend (pctxt->ctxt->bus_ids,
                                          GUINT_TO_POINTER (bus_id));

out:
  g_free (object_path);
  return G_SOURCE_REMOVE;
}

static void
on_page_created (WebKitWebExtension *extension,
                 WebKitWebPage      *page,
                 Context            *ctxt)
{
  PageContext *pctxt = g_new0 (PageContext, 1);
  pctxt->ctxt = ctxt;
  pctxt->page = page;

  ctxt->page_ctxts = g_slist_prepend (ctxt->page_ctxts, pctxt);

  g_idle_add_full (G_PRIORITY_HIGH_IDLE, (GSourceFunc) register_object,
                   pctxt, NULL);
}

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 Context         *ctxt)
{
  GError *error = NULL;

  ctxt->connection = connection;

  /* Export our interface on the bus */
  ctxt->node = g_dbus_node_info_new_for_xml (introspection_xml, &error);
  if (ctxt->node == NULL)
    goto fail;
  ctxt->interface = g_dbus_node_info_lookup_interface (ctxt->node,
                                                       WH2_DBUS_INTERFACE_NAME);
  if (ctxt->interface == NULL)
    goto fail;

  return;

fail:
  if (error != NULL)
    {
      g_critical ("Error hooking up web extension DBus interface: %s",
                  error->message);
      g_clear_error (&error);
    }
  else
    {
      g_critical ("Unknown error hooking up web extension DBus interface");
    }
}

static void
unregister_object (gpointer         data,
                   GDBusConnection *connection)
{
  guint bus_id = GPOINTER_TO_UINT (data);
  if (!g_dbus_connection_unregister_object (connection, bus_id))
    g_critical ("Trouble unregistering object");
}

static void
on_name_lost (GDBusConnection *connection,
              const gchar     *name,
              Context         *ctxt)
{
  if (connection == NULL)
    {
      g_warning ("Could not initialize DBus interface for WebHelper2 "
                 "extension; the name %s was lost.", name);
      return;
    }

  g_slist_foreach (ctxt->bus_ids, (GFunc) unregister_object, connection);
}

/* Receives the main program's unique DBus name as user data. */
G_MODULE_EXPORT void
webkit_web_extension_initialize_with_user_data (WebKitWebExtension *extension,
                                                const GVariant     *data_from_app)
{
  const gchar *name = g_variant_get_string ((GVariant *) data_from_app, NULL);

  Context *ctxt = g_new0 (Context, 1);
  ctxt->main_program_name = g_strdup (name);
  gchar *well_known_name = g_strconcat (name, ".webhelper", NULL);

  g_signal_connect (extension, "page-created",
                    G_CALLBACK (on_page_created), ctxt);

  g_bus_own_name (G_BUS_TYPE_SESSION, well_known_name,
                  G_BUS_NAME_OWNER_FLAGS_NONE,
                  (GBusAcquiredCallback) on_bus_acquired,
                  NULL, /* name acquired callback */
                  (GBusNameLostCallback) on_name_lost,
                  ctxt, (GDestroyNotify) context_free);

  g_free (well_known_name);
}
