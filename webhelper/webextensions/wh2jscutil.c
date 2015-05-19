/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#include <glib.h>
#include <JavaScriptCore/JavaScript.h>

#include "wh2jscutil.h"

G_GNUC_INTERNAL
gboolean
set_object_property  (JSContextRef         js,
                      JSObjectRef          object,
                      const gchar         *property_name,
                      JSValueRef           property_value,
                      JSPropertyAttributes flags)
{
  JSValueRef exception = NULL;
  JSStringRef property_name_ref = JSStringCreateWithUTF8CString (property_name);
  JSObjectSetProperty (js, object, property_name_ref, property_value, flags,
                       &exception);
  JSStringRelease (property_name_ref);
  if (exception != NULL)
    {
      g_critical ("There was a problem setting the property '%s'.",
                  property_name);
      return FALSE;
    }
  return TRUE;
}

/* Returns a newly allocated string. */
G_GNUC_INTERNAL
gchar *
string_ref_to_string (JSStringRef string_ref)
{
  size_t bufsize = JSStringGetMaximumUTF8CStringSize (string_ref);
  gchar *string = g_new0 (gchar, bufsize);
  JSStringGetUTF8CString (string_ref, string, bufsize);
  return string;
}

G_GNUC_INTERNAL
JSValueRef
string_to_value_ref (JSContextRef js,
                     const gchar *string)
{
  JSStringRef string_ref = JSStringCreateWithUTF8CString (string);
  JSValueRef value_ref = JSValueMakeString (js, string_ref);
  /* value_ref owns string_ref now */
  return value_ref;
}

G_GNUC_INTERNAL
JSValueRef
throw_exception (JSContextRef js,
                 const gchar  *message)
{
  JSValueRef msgval = string_to_value_ref (js, message);
  JSValueRef inner_error = NULL;
  JSObjectRef exception = JSObjectMakeError (js, 1, &msgval, &inner_error);
  if (inner_error != NULL)
    return inner_error;
  return (JSValueRef) exception;
}
