/* Copyright 2015 Endless Mobile, Inc. */

#include "config.h"
#include <gio/gio.h>
#include <glib.h>
#include <glib/gi18n-lib.h>
#include <string.h>

#include "eoslicense.h"

/**
 * SECTION:license
 * @short_description: License provider for applications
 * @title: License
 *
 * It provides an API to serve license information and files to applications.
 */

/* These are the recognized string values for the "license" field. Any other
license must be clarified in the comments, or linked to with the "license_uri"
field. Make sure to add new values to the table "image-attribution-licenses" in
the documentation of EosApplication and to the two arrays below this one. */
gchar * const recognized_licenses[] = {
  "Public domain",
  "Owner permission",
  "Copyrighted free use",
  "CC0 1.0",
  "CC BY 2.0",
  "CC BY 3.0",
  "CC BY 4.0",
  "CC BY-NC 2.0",
  "CC BY-NC 3.0",
  "CC BY-NC-SA 2.0",
  "CC BY-ND 2.0",
  "CC BY-ND 3.0",
  "CC BY-SA 2.0",
  "CC BY-SA 2.5",
  "CC BY-SA 3.0",
  "CC BY-SA 4.0",
  "GFDL 1.1",
  "GFDL 1.2",
  "GFDL 1.3",
  NULL
};

/* These are the recognized display names for license names. There is a 1-to-1
correspondence between `recognized_licenses` and this array */
gchar * const recognized_licenses_display_names[] = {
  /* TRANSLATORS: These names should be translated as the official names of the
  licenses in your language. Note: these names are markup, and so should not
  contain any ampersands (&), less-than signs (<) or greater-than signs (>). */
  N_("Public domain"),
  N_("Owner permission"),
  N_("Copyrighted free use"),
  N_("CC0-1.0"),
  N_("Creative Commons Attribution 2.0"),
  N_("Creative Commons Attribution 3.0"),
  N_("Creative Commons Attribution 4.0"),
  N_("Creative Commons Attribution-NonCommercial 2.0"),
  N_("Creative Commons Attribution-NonCommercial 3.0"),
  N_("Creative Commons Attribution-NonCommercial-ShareAlike 2.0 Generic"),
  N_("Creative Commons Attribution-NoDerivs 2.0"),
  N_("Creative Commons Attribution-NoDerivs 3.0"),
  N_("Creative Commons Attribution-ShareAlike 2.0"),
  N_("Creative Commons Attribution-ShareAlike 2.5 Generic"),
  N_("Creative Commons Attribution-ShareAlike 3.0"),
  N_("Creative Commons Attribution-ShareAlike 4.0 International"),
  N_("GNU Free Documentation License, version 1.1"),
  N_("GNU Free Documentation License, version 1.2"),
  N_("GNU Free Documentation License, version 1.3"),
  NULL
};

/* These are the actual filenames for license files. There is a 1-to-1
correspondence between `recognized_licenses` and this array. */
static gchar * const recognized_licenses_filenames[] = {
  NULL,
  NULL,
  NULL,
  "publicdomain/CC0-1.0.html",
  "creativecommons/CC-BY-2.0.html",
  "creativecommons/CC-BY-3.0.html",
  "creativecommons/CC-BY-4.0.html",
  "creativecommons/CC-BY-NC-2.0.html",
  "creativecommons/CC-BY-NC-3.0.html",
  "creativecommons/CC-BY-NC-SA-2.0.html",
  "creativecommons/CC-BY-ND-2.0.html",
  "creativecommons/CC-BY-ND-3.0.html",
  "creativecommons/CC-BY-SA-2.0.html",
  "creativecommons/CC-BY-SA-2.5.html",
  "creativecommons/CC-BY-SA-3.0.html",
  "creativecommons/CC-BY-SA-4.0.html",
  "gnu/FDL-1.1.html",
  "gnu/FDL-1.2.html",
  "gnu/FDL-1.3.html",
  NULL
};

static const char *
get_locale (GFile *cc_licenses_dir)
{
  static gchar * locale;

  if (g_once_init_enter (&locale))
    {
      const gchar * const * languages = g_get_language_names ();
      const gchar * const * iter;
      for (iter = languages; *iter != NULL; iter++)
        {
          GFile *license_file = g_file_get_child (cc_licenses_dir, *iter);

          gboolean locale_file_exists = g_file_query_exists (license_file, NULL);

          g_object_unref (license_file);

          if (locale_file_exists)
            break;
        }

      /* Licenses will always be installed for at least one locale, which
      may be the default C locale. */
      g_assert (*iter != NULL);

      g_once_init_leave (&locale, *iter);
    }

  return locale;
}

static gchar *
get_sanitized_license_code (const gchar *license)
{
  gchar *sanitized_license = g_strdup (license);

  /* Checks whether the prefix of license is "CC-BY", and if so, converts it
  to "CC BY". */
  if (g_str_has_prefix (sanitized_license, "CC-BY"))
    sanitized_license[2] = ' ';
  return sanitized_license;
}

static int
get_license_index (const gchar *license)
{
  int i;
  for (i = 0; recognized_licenses[i] != NULL; i++)
    {
      if (strcmp (recognized_licenses[i], license) == 0)
        return i;
    }

  /* If no license was found, return -1. */
  return -1;
}

/**
 * eos_get_license_display_name:
 * @license: The license name
 *
 * Retrieves the display name for the specified license and the current locale.
 *
 * Returns: A string for the specified @license name and the
 *          current locale.
 * Since: 0.4
 */
const gchar *
eos_get_license_display_name (const gchar *license)
{
  /* Sanitize input */
  gchar *sanitized_license = get_sanitized_license_code (license);
  /* Get index of valid license */
  int index = get_license_index (sanitized_license);
  g_free (sanitized_license);

  /* If the array value is null, it means we don't have a license file for that
  license name. */
  if (recognized_licenses[index] == NULL)
    return _("Unknown license");

  return gettext (recognized_licenses_display_names[index]);
}

/**
 * eos_get_license_file:
 * @license: The license name
 *
 * Retrieves a GFile for the specified licene and the current locale.
 *
 * Returns: (transfer full) (allow-none): A GFile for the specified @license
 *                                        name and the current locale. It
 *                                        returns %NULL if the license is not
 *                                        found or does not have an associated
 *                                        file.
 * Since: 0.4
 */
GFile *
eos_get_license_file (const gchar *license)
{
  /* Sanitize input */
  gchar *sanitized_license = get_sanitized_license_code (license);
  /* Get index of valid license */
  int index = get_license_index (sanitized_license);
  g_free (sanitized_license);

  /* If the array value is null, it means we don't have a license file for that
  license name. */
  if (recognized_licenses_filenames[index] == NULL)
    return NULL;

  /* We need to split the recognized_licenses_filenames into the subdir and the filename */
  gchar **license_filename = g_strsplit (recognized_licenses_filenames[index], "/", 2);

  gchar *licenses_path = g_build_filename (DATADIR, "licenses",
                                           license_filename[0], NULL);
  GFile *cc_licenses_dir = g_file_new_for_path (licenses_path);
  g_free (licenses_path);

  const char *locale = get_locale (cc_licenses_dir);

  GFile *license_locale_dir = g_file_get_child (cc_licenses_dir, locale);
  GFile *license_file = g_file_get_child (license_locale_dir,
                                          license_filename[1]);

  g_object_unref (cc_licenses_dir);
  g_object_unref (license_locale_dir);
  g_strfreev (license_filename);

  return license_file;
}
