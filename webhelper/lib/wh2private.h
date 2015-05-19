/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/* Copyright 2015 Endless Mobile, Inc. */

#ifndef WH2_PRIVATE_H
#define WH2_PRIVATE_H

#include <glib.h>
#include <webkit2/webkit2.h>

G_BEGIN_DECLS

void wh2_register_uri_scheme (const gchar                   *scheme,
                              WebKitURISchemeRequestCallback callback,
                              gpointer                       user_data,
                              GDestroyNotify                 notify);

G_END_DECLS

#endif /* WH2_PRIVATE_H */
