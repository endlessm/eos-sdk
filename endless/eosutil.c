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
  // Composite mode support has been removed from Endless OS
  // <https://phabricator.endlessm.com/T22102>
  return FALSE;
}
