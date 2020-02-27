#include <glib.h>

#include "endless.h"

/**
 * eos_is_composite_tv_monitor:
 * @monitor: (allow-none): a #GdkMonitor, or %NULL to use the default display's primary monitor.
 *
 * Determines whether @monitor is a composite TV out.
 *
 * Returns: %TRUE if @monitor is a composite TV, otherwise %FALSE.
 *
 * Since: 0.6
 */
gboolean
eos_is_composite_tv_monitor (GdkMonitor *monitor)
{
  GdkDisplay *gdk_display = gdk_display_get_default ();

  if (monitor == NULL)
    monitor = gdk_display_get_primary_monitor (gdk_display);

  if (monitor == NULL)
    monitor = gdk_display_get_monitor (gdk_display, 0);

  if (monitor == NULL)
    return FALSE;

  GdkRectangle geom;
  gdk_monitor_get_geometry (monitor, &geom);

  int scale = gdk_monitor_get_scale_factor (monitor);
  int device_width_px = geom.width * scale;
  int device_height_px = geom.height * scale;

  if (device_width_px != 720)
    return FALSE;

  if (device_height_px != 480 && device_height_px != 576)
    return FALSE;

  g_debug ("Composite screen detected for monitor %p", monitor);
  return TRUE;
}

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
