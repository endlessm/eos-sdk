/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#include <glib.h>
#include <webkit2/webkit2.h>

#include "wh2private.h"

/**
 * wh2_private_register_global_uri_scheme:
 * @scheme: the network scheme to register
 * @callback: a #WebKitURISchemeRequestCallback.
 * @user_data: (closure): user data for the @callback
 * @notify: destroy notify function for the @callback
 *
 * Workaround for https://bugzilla.gnome.org/show_bug.cgi?id=729611
 *
 * Registers a URI scheme handler with the default WebContext. Does not pass the
 * GDestroyNotifyFunc, which GJS uses to shim a destructor for @callback, along
 * to the the web context.
 *
 * The default web context is a global object which does not get destroyed
 * until a atexit handler after the javascript runtime has been torn down.
 * Calling into the GJS function destructor at that point would be a
 * mistake.
 */
void
wh2_register_uri_scheme (const gchar                   *scheme,
                         WebKitURISchemeRequestCallback callback,
                         gpointer                       user_data,
                         GDestroyNotify                 notify)
{
  WebKitWebContext *context = webkit_web_context_get_default ();
  webkit_web_context_register_uri_scheme (context, scheme, callback, NULL, NULL);
}
