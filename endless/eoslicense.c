/* Copyright 2015 Endless Mobile, Inc. */

#include <gio/gio.h>
#include <glib.h>
#include <string.h>

#include "eoslicense.h"
#include "eoslicense-private.h"

/**
 * SECTION:
 */

/* These are the recognized string values for the "license" field. Any other
license must be clarified in the comments, or linked to with the "license_uri"
field. Make sure to add new values to the table "image-attribution-licenses" in
the documentation of EosApplication and to the two arrays below this one. */
char * const recognized_licenses[] = {
  "Public domain",
  "Owner permission",
  "CC-BY 2.0",
  "CC-BY 3.0",
  "CC-BY 4.0",
  "CC-BY-NC 2.0",
  "CC-BY-NC 3.0",
  "CC-BY-NC-SA 2.0",
  "CC-BY-ND 2.0",
  "CC-BY-ND 3.0",
  "CC-BY-SA 2.0",
  "CC-BY-SA 2.5",
  "CC-BY-SA 3.0",
  "CC-BY-SA 4.0",
  NULL
};

/* These are the actual filenames for license files. There is a 1-to-1
correspondence between these two arrays. */
static gchar * const recognized_licenses_filenames[] = {
  NULL,
  NULL,
  "CC-BY-2.0.html",
  "CC-BY-3.0.html",
  "CC-BY-4.0.html",
  "CC-BY-NC-2.0.html",
  "CC-BY-NC-3.0.html",
  "CC-BY-NC-SA-2.0.html",
  "CC-BY-ND-2.0.html",
  "CC-BY-ND-3.0.html",
  "CC-BY-SA-2.0.html",
  "CC-BY-SA-2.5.html",
  "CC-BY-SA-3.0.html",
  "CC-BY-SA-4.0.html",
  NULL
};

static const char *
get_locale (GFile *cc_licenses_dir)
{
  static gchar * locale;

  if (g_once_init_enter (&locale))
    {
      const gchar * const * languages = g_get_language_names();
      const gchar * const * iter;
      for (iter=languages; *iter!= NULL; iter++)
        {
          GFile *license_file = g_file_get_child(cc_licenses_dir, *iter);

          if (g_file_query_exists(license_file, NULL))
            {
              g_object_unref(license_file);
              break;
            }

          g_object_unref(license_file);
        }

      g_assert(*iter != NULL);

      g_once_init_leave (&locale, *iter);
    }

  return locale;
}

/**
 * eos_get_license_file:
 * @license: The license name
 *
 * Returns:(transfer full): a GFile for the specified @license file and the
 *                          current locale.
 */
GFile *
eos_get_license_file (const gchar *license)
{
  // Check license is valid
  int i;
  for(i=0; recognized_licenses[i]!=NULL; i++)
    {
      if (strcmp(recognized_licenses[i], license) == 0)
        {
          break;
        }
    }

  g_print("%s\n", license);
  g_print("recognized_licenses[i]=%s\n", recognized_licenses[i]);
  g_return_val_if_fail(recognized_licenses[i]!=NULL, NULL);

  // If the array value is null, it means we don't have a license file for that
  // license name.
  if (recognized_licenses_filenames[i] == NULL)
    return NULL;

  gchar *licenses_path = g_build_filename(DATADIR, "licenses", "creativecommons",
                                          NULL);
  GFile *cc_licenses_dir = g_file_new_for_path(licenses_path);
  g_free(licenses_path);

  const char * locale = get_locale(cc_licenses_dir);

  GFile *license_locale_dir = g_file_get_child(cc_licenses_dir, locale);
  GFile *license_file = g_file_get_child (license_locale_dir,
                                          recognized_licenses_filenames[i]);

  g_object_unref(license_locale_dir);

  return license_file;
}
