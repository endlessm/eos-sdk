/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#include <math.h>
#include <string.h>

#include <glib.h>
#include <JavaScriptCore/JavaScript.h>
#include <webkit2/webkit-web-extension.h>
#include <webkitdom/webkitdom.h>

#include "wh2jscutil.h"

#define PRIVATE_NAME "_webhelper_private"
#define WH2_DBUS_INTERFACE_NAME "com.endlessm.WebHelper2.Translation"
#define MAIN_PROGRAM_OBJECT_PATH "/com/endlessm/gettext"
#define MAIN_PROGRAM_INTERFACE_NAME "com.endlessm.WebHelper2.Gettext"

/* Declaration of externally visible symbol */
void webkit_web_extension_initialize_with_user_data (WebKitWebExtension *, const GVariant *);

typedef struct {
  WebKitWebExtension *extension;  /* unowned */
  GDBusConnection *connection;  /* unowned */
  GDBusNodeInfo *node;  /* owned */
  GDBusInterfaceInfo *interface;  /* owned by node */
  GSList *bus_ids;  /* GSList<guint>; owned */
  GArray *unregistered_pages;  /* GArray<guint64>; owned */
  gchar *main_program_name;  /* owned; well-known-name of main program */
} Context;

typedef struct {
  Context *ctxt;  /* unowned */
  guint64 page_id;
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
  if (ctxt->unregistered_pages != NULL)
    g_array_free (ctxt->unregistered_pages, TRUE);
  ctxt->unregistered_pages = NULL;
  g_clear_pointer (&ctxt->main_program_name, g_free);
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

static gchar *
ngettext_translation_function (const gchar *singular,
                               const gchar *plural,
                               guint64      number,
                               Context     *ctxt)
{
  GError *error = NULL;
  GVariant *result =
    g_dbus_connection_call_sync (ctxt->connection, ctxt->main_program_name,
                                 MAIN_PROGRAM_OBJECT_PATH,
                                 MAIN_PROGRAM_INTERFACE_NAME, "NGettext",
                                 g_variant_new ("(sst)", singular, plural,
                                                number),
                                 (GVariantType *) "(s)",
                                 G_DBUS_CALL_FLAGS_NO_AUTO_START,
                                 -1 /* timeout */, NULL /* cancellable */,
                                 &error);
  if (result == NULL)
    {
      g_warning ("No return value from ngettext: %s", error->message);
      g_clear_error (&error);
      return g_strdup (number == 1 ? singular : plural);
    }

  gchar *retval;
  g_variant_get (result, "(s)", &retval);
  g_variant_unref (result);
  return retval;
}

static JSValueRef
gettext_shim (JSContextRef     js,
              JSObjectRef      function,
              JSObjectRef      this_object,
              size_t           n_args,
              const JSValueRef args[],
              JSValueRef      *exception)
{
  if (n_args != 1)
    {
      gchar *errmsg = g_strdup_printf ("Expected one argument to gettext(),"
                                       "but got %d.", n_args);
      *exception = throw_exception (js, errmsg);
      g_free (errmsg);
      return NULL;
    }
  if (!JSValueIsString (js, args[0]))
    {
      *exception = throw_exception (js,
                                    "Expected a string argument to gettext().");
      return NULL;
    }

  JSObjectRef window = JSContextGetGlobalObject (js);
  JSStringRef private_name = JSStringCreateWithUTF8CString (PRIVATE_NAME);
  JSValueRef private_data = JSObjectGetProperty (js, window, private_name,
                                                 exception);
  if (JSValueIsUndefined (js, private_data))
    return NULL;  /* propagate exception */
  Context *ctxt = (Context *) JSObjectGetPrivate ((JSObjectRef) private_data);

  JSStringRef message_ref = JSValueToStringCopy (js, args[0], exception);
  if (message_ref == NULL)
    return NULL;  /* propagate exception */
  gchar *message = string_ref_to_string (message_ref);
  JSStringRelease (message_ref);

  gchar *translation = translation_function (message, ctxt);
  g_free (message);

  JSValueRef retval = string_to_value_ref (js, translation);
  g_free (translation);
  return retval;
}

static JSValueRef
ngettext_shim (JSContextRef     js,
               JSObjectRef      function,
               JSObjectRef      this_object,
               size_t           n_args,
               const JSValueRef args[],
               JSValueRef      *exception)
{
  if (n_args != 3)
    {
      gchar *errmsg = g_strdup_printf ("Expected three arguments to ngettext(),"
                                       "but got %d.", n_args);
      *exception = throw_exception (js, errmsg);
      g_free (errmsg);
      return NULL;
    }
  if (!JSValueIsString (js, args[0]))
    {
      *exception = throw_exception (js, "The first argument to ngettext() "
                                    "must be a string.");
      return NULL;
    }
  if (!JSValueIsString (js, args[1]))
    {
      *exception = throw_exception (js, "The second argument to ngettext() "
                                    "must be a string.");
      return NULL;
    }
  if (!JSValueIsNumber (js, args[2]))
    {
      *exception = throw_exception (js, "The third argument to ngettext() "
                                    "must be a number.");
      return NULL;
    }

  JSObjectRef window = JSContextGetGlobalObject (js);
  JSStringRef private_name = JSStringCreateWithUTF8CString (PRIVATE_NAME);
  JSValueRef private_data = JSObjectGetProperty (js, window, private_name,
                                                 exception);
  if (JSValueIsUndefined (js, private_data))
    return NULL;  /* propagate exception */
  Context *ctxt = (Context *) JSObjectGetPrivate ((JSObjectRef) private_data);

  JSStringRef singular_ref = JSValueToStringCopy (js, args[0], exception);
  if (singular_ref == NULL)
    return NULL;  /* propagate exception */
  gchar *singular_msg = string_ref_to_string (singular_ref);
  JSStringRelease (singular_ref);

  JSStringRef plural_ref = JSValueToStringCopy (js, args[1], exception);
  if (plural_ref == NULL)
    {
      g_free (singular_msg);
      return NULL;  /* propagate exception */
    }
  gchar *plural_msg = string_ref_to_string (plural_ref);
  JSStringRelease (plural_ref);

  double number = JSValueToNumber (js, args[2], exception);
  if (isnan (number))
    {
      g_free (singular_msg);
      g_free (plural_msg);
      return NULL;  /* propagate exception */
    }

  gchar *translation = ngettext_translation_function (singular_msg, plural_msg,
                                                      (guint64) number, ctxt);
  g_free (singular_msg);
  g_free (plural_msg);

  JSValueRef retval = string_to_value_ref (js, translation);
  g_free (translation);
  return retval;
}

static gchar *
normalize_string (const gchar *string)
{
  static GRegex *whitespace = NULL;

  if (g_once_init_enter (&whitespace))
    {
      GError *regex_error = NULL;
      GRegex *new_regex = g_regex_new ("\\s+", G_REGEX_OPTIMIZE, 0,
                                       &regex_error);
      // Don't free; will persist until exit
      if (new_regex == NULL)
        {
          g_critical ("Trouble creating regex: %s\n", regex_error->message);
          g_clear_error (&regex_error);
        }

      g_once_init_leave (&whitespace, new_regex);
    }

  GError *error = NULL;
  gchar *copy = g_strstrip (g_strdup (string));
  gchar *retval = g_regex_replace_literal (whitespace, copy, -1, 0, " ", 0, &error);
  if (retval == NULL)
    {
      g_critical ("Trouble normalizing string: %s\n", error->message);
      g_clear_error (&error);
      return copy;
    }
  g_free (copy);
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
          gchar *normalized = normalize_string (inner_html);
          gchar *translated_html = translation_function (normalized, ctxt);
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
          g_free (normalized);
        }
      else
        {
          gchar *text = webkit_dom_node_get_text_content (element);
          gchar *normalized = normalize_string (text);
          gchar *translated_text = translation_function (normalized, ctxt);
          webkit_dom_node_set_text_content (element, translated_text, &error);
          if (error != NULL)
            {
              g_warning ("There was a problem translating '%s' to '%s': %s",
                         text, translated_text, error->message);
              g_clear_error (&error);
            }

          g_free (translated_text);
          g_free (text);
          g_free (normalized);
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

  WebKitWebPage *page = webkit_web_extension_get_page (pctxt->ctxt->extension,
                                                       pctxt->page_id);
  if (page == NULL)
    return;
  /* The page may have been destroyed, but WebKit doesn't let us find out. */

  WebKitDOMDocument *document = webkit_web_page_get_dom_document (page);
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

static void
register_object (guint64  page_id,
                 Context *ctxt)
{
  GError *error = NULL;

  g_assert (ctxt->connection != NULL);

  gchar *object_path = g_strdup_printf("/com/endlessm/webview/%"
                                       G_GUINT64_FORMAT, page_id);

  /* This struct is owned by the registered DBus object */
  PageContext *pctxt = g_new0 (PageContext, 1);
  pctxt->ctxt = ctxt;
  pctxt->page_id = page_id;

  guint bus_id =
    g_dbus_connection_register_object (ctxt->connection, object_path,
                                       ctxt->interface, &dbus_impl_vtable,
                                       pctxt, (GDestroyNotify) g_free, &error);
  g_free (object_path);
  if (bus_id == 0)
    {
      g_critical ("Failed to export webview object on bus: %s", error->message);
      g_clear_error (&error);
      goto fail;
    }

  ctxt->bus_ids = g_slist_prepend (ctxt->bus_ids, GUINT_TO_POINTER (bus_id));
  return;

fail:
  g_free (pctxt);
}

static void
on_page_created (WebKitWebExtension *extension,
                 WebKitWebPage      *page,
                 Context            *ctxt)
{
  /* The ID is known to the main process and the web process. So we can address
  a specific web page over DBus. */
  guint64 id = webkit_web_page_get_id (page);

  if (ctxt->connection == NULL)
    {
      /* The connection is not ready yet. Save the page ID in a list of pages
      for which we need to register objects later. */
      g_array_append_val (ctxt->unregistered_pages, id);
      return;
    }

  register_object (id, ctxt);
}

/* window-object-cleared is the best time to define properties on the page's
window object, according to the documentation. */
static void
on_window_object_cleared (WebKitScriptWorld *script_world,
                          WebKitWebPage     *page,
                          WebKitFrame       *frame,
                          Context           *ctxt)
{
  JSGlobalContextRef js =
    webkit_frame_get_javascript_context_for_script_world (frame, script_world);
  JSObjectRef window = JSContextGetGlobalObject (js);

  /* First we need to create a custom class for a private data object to store
  our context in, because you can't pass callback data to JavaScriptCore
  callbacks. You also can't set private data on a Javascript object if it's not
  of a custom class, because the built-in classes don't allocate space for a
  private pointer. */
  JSClassDefinition class_def = {
    .className = "PrivateContextObject"
  };
  JSClassRef klass = JSClassCreate (&class_def);
  JSObjectRef private_data = JSObjectMake (js, klass, ctxt);
  JSClassRelease (klass);

  if (!set_object_property (js, window, PRIVATE_NAME, (JSValueRef) private_data,
                            kJSPropertyAttributeReadOnly | kJSPropertyAttributeDontEnum | kJSPropertyAttributeDontDelete))
    return;

  JSObjectRef gettext_func =
    JSObjectMakeFunctionWithCallback (js, NULL, gettext_shim);
  if (!set_object_property (js, window, "gettext", (JSValueRef) gettext_func,
                            kJSPropertyAttributeNone))
    return;

  JSObjectRef ngettext_func =
    JSObjectMakeFunctionWithCallback (js, NULL, ngettext_shim);
  if (!set_object_property (js, window, "ngettext", (JSValueRef) ngettext_func,
                            kJSPropertyAttributeNone))
    return;
}

static void
on_bus_acquired (GDBusConnection *connection,
                 const gchar     *name,
                 Context         *ctxt)
{
  GError *error = NULL;

  ctxt->connection = connection;

  /* Get a notification when Javascript is ready */
  WebKitScriptWorld *script_world = webkit_script_world_get_default ();
  g_signal_connect (script_world, "window-object-cleared",
                    G_CALLBACK (on_window_object_cleared), ctxt);

  /* Export our interface on the bus */
  ctxt->node = g_dbus_node_info_new_for_xml (introspection_xml, &error);
  if (ctxt->node == NULL)
    goto fail;
  ctxt->interface = g_dbus_node_info_lookup_interface (ctxt->node,
                                                       WH2_DBUS_INTERFACE_NAME);
  if (ctxt->interface == NULL)
    goto fail;

  /* Register DBus objects for any pages that were created before we got here */
  guint ix;
  for (ix = 0; ix < ctxt->unregistered_pages->len; ix++)
    {
      guint64 id = g_array_index (ctxt->unregistered_pages, guint64, ix);
      register_object (id, ctxt);
    }
  g_array_remove_range (ctxt->unregistered_pages, 0,
                        ctxt->unregistered_pages->len);

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
  ctxt->extension = extension;
  ctxt->unregistered_pages = g_array_new (FALSE, FALSE, sizeof (guint64));
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
