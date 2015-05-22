/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#ifndef WH2_JSC_UTIL_H
#define WH2_JSC_UTIL_H

#include <glib.h>
#include <JavaScriptCore/JavaScript.h>

gboolean   set_object_property  (JSContextRef         js,
                                 JSObjectRef          object,
                                 const gchar         *property_name,
                                 JSValueRef           property_value,
                                 JSPropertyAttributes flags);

gchar     *string_ref_to_string (JSStringRef          string_ref);

JSValueRef string_to_value_ref  (JSContextRef         js,
                                 const gchar         *string);

JSValueRef throw_exception      (JSContextRef         js,
                                 const gchar         *message);

#endif /* WH2_JSC_UTIL_H */
