#include <glib.h>

#include "endless.h"

/**
 * eos_get_system_personality:
 *
 * Retrieves the "personality" of the system.
 *
 * The personality is a unique string that identifies the installation
 * of EndlessOS for a specific country or audience. The availability of
 * certain applications, or their content, is determined by this value.
 *
 * Return value: (transfer none): a string, owned by the Endless SDK,
 *   with the name of the personality. You should never free or modify
 *   the returned string.
 */
const gchar *
eos_get_system_personality (void)
{
  static gchar *personality;

  if (g_once_init_enter (&personality))
    {
      gchar *tmp;

      tmp = g_strdup (g_getenv ("ENDLESS_OS_PERSONALITY"));
      if (tmp != NULL && tmp[0] == '\0')
        {
          g_free (tmp);
          tmp = NULL;
        }

      if (tmp == NULL)
        {
          GKeyFile *personality_file = g_key_file_new ();
          char *path = g_build_filename (SYSCONFDIR,
                                         "EndlessOS",
                                         "personality.conf",
                                         NULL);

          GError *error = NULL;
          g_key_file_load_from_file (personality_file, path,
                                     G_KEY_FILE_NONE, &error);

          if (error == NULL)
            tmp = g_key_file_get_string (personality_file, "Personality",
                                         "PersonalityName", &error);

          if (error != NULL)
            {
              g_critical ("No personality defined: %s", error->message);
              g_error_free (error);
              tmp = NULL;
            }

          g_key_file_free (personality_file);
          g_free (path);
        }

      if (tmp == NULL)
        tmp = g_strdup ("default");

      g_once_init_leave (&personality, tmp);
    }

  return personality;
}

/**
 * eos_is_composite_tv_screen:
 * @screen: a #GdkScreen, or %NULL to use the default display's default screen.
 *
 * Determines whether @screen is a composite TV out.
 *
 * Returns: %TRUE if @screen is a composite TV, otherwise %FALSE.
 *
 * Since: 0.6
 */
gboolean
eos_is_composite_tv_screen (GdkScreen *screen)
{
  if (screen == NULL)
    screen = gdk_screen_get_default ();

  if (gdk_screen_get_width (screen) != 720)
    return FALSE;

  int height = gdk_screen_get_height (screen);
  if (height != 480 && height != 576)
    return FALSE;

  g_debug ("Composite screen detected for screen %p", screen);
  return TRUE;
}
