#include <glib.h>

#include "endless.h"

/**
 * eos_is_composite_tv_screen:
 * @screen: (allow-none): a #GdkScreen, or %NULL to use the default display's default screen.
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
