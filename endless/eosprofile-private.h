/* Copyright 2017  Endless */

#pragma once

#include "eosprofile.h"

G_BEGIN_DECLS

/* Increase every time the probe format changes */
#define PROBE_DB_VERSION                1

#define PROBE_DB_META_BASE_KEY          "/com/endlessm/Sdk/meta"
#define PROBE_DB_META_VERSION_KEY       PROBE_DB_META_BASE_KEY "/db_version"

typedef struct {
  GHashTable *probes;

  gboolean capture;
  char *capture_file;
} ProfileState;

G_LOCK_DEFINE_STATIC (profile_state);
static ProfileState *profile_state;

typedef struct {
  gint64 start_time;
  gint64 end_time;
} ProfileSample;

void
eos_profile_state_init (void);

void
eos_profile_state_dump (void);

G_END_DECLS
