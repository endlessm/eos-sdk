/* Copyright 2013 Endless Mobile, Inc. */

#include "config.h"
#include "eossplashpagemanager.h"

#include <gtk/gtk.h>

#include <string.h>

/**
 * SECTION:splash-page-manager
 * @short_description: Add splash screen to application
 * @title: Splash Page Manager
 *
 * Builds on the #EosPageManager to provide an application with two pages.
 * The first page is called the splash page and is the first page to be
 * presented to the user. This is generally a simple page with a minimal
 * interface that acts as a gateway to the rest of the application.
 *
 * The second page, called the main page, is generally where the main work of
 * the application will get done. This can be shown instead of the splash page
 * at any time by calling eos_splash_page_manager_show_main_page(). The main
 * page can be a second page manager (such as a tabbed notebook view), for
 * applications with more complex page flow.
 *
 * The splash screen and main page can contain any widget. Call
 * eos_splash_page_manager_show_main_page() and
 * eos_splash_page_manager_show_splash_page() to toggle between the two views.
 * The splash screen will be shown by default until a call to
 * eos_splash_page_manager_show_main_page() is made.
 */

G_DEFINE_TYPE (EosSplashPageManager, eos_splash_page_manager, EOS_TYPE_PAGE_MANAGER)

#define SPLASH_PAGE_MANAGER_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), EOS_TYPE_SPLASH_PAGE_MANAGER, EosSplashPageManagerPrivate))

struct _EosSplashPageManagerPrivate
{
  GtkWidget *splash_page;
  GtkWidget *main_page;
  gboolean main_page_shown;
};

enum
{
  PROP_0,
  PROP_SPLASH_PAGE,
  PROP_MAIN_PAGE,
  NPROPS
};

static GParamSpec *eos_splash_page_manager_props[NPROPS] = { NULL, };

static void
eos_splash_page_manager_get_property (GObject    *object,
                                      guint       property_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  EosSplashPageManager *self = EOS_SPLASH_PAGE_MANAGER (object);

  switch (property_id)
    {
    case PROP_SPLASH_PAGE:
      g_value_set_object (value, eos_splash_page_manager_get_splash_page (self));
      break;

    case PROP_MAIN_PAGE:
      g_value_set_object (value, eos_splash_page_manager_get_main_page (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_splash_page_manager_set_property (GObject      *object,
                                      guint         property_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  EosSplashPageManager *self = EOS_SPLASH_PAGE_MANAGER (object);

  switch (property_id)
    {
    case PROP_SPLASH_PAGE:
      eos_splash_page_manager_set_splash_page (self, g_value_get_object (value));
      break;

    case PROP_MAIN_PAGE:
      eos_splash_page_manager_set_main_page (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
    }
}

static void
eos_splash_page_manager_class_init (EosSplashPageManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (EosSplashPageManagerPrivate));

  object_class->get_property = eos_splash_page_manager_get_property;
  object_class->set_property = eos_splash_page_manager_set_property;

  /**
   * EosSplashPageManager:splash-page:
   *
   * A reference to the splash page widget of the splash page manager. If no
   * page has been set, points to null.
   */
  eos_splash_page_manager_props[PROP_SPLASH_PAGE] =
    g_param_spec_object ("splash-page", "Splash page",
                         "Splash page of the splash page manager",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  /**
   * EosSplashPageManager:main-page:
   *
   * A reference to the main page widget of the splash page manager. If no
   * page has been set, points to null.
   */
  eos_splash_page_manager_props[PROP_MAIN_PAGE] =
    g_param_spec_object ("main-page", "Main page",
                         "Main page of the splash page manager",
                         GTK_TYPE_WIDGET,
                         G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (object_class, NPROPS,
                                     eos_splash_page_manager_props);
}

static void
eos_splash_page_manager_init (EosSplashPageManager *self)
{
  self->priv = SPLASH_PAGE_MANAGER_PRIVATE (self);
}

/* Public API */

/**
 * eos_splash_page_manager_new:
 *
 * Creates a new default splash page manager.
 *
 * Returns: the new page manager.
 */
GtkWidget *
eos_splash_page_manager_new (void)
{
  return g_object_new (EOS_TYPE_SPLASH_PAGE_MANAGER, NULL);
}

/**
 * eos_splash_page_manager_get_splash_page:
 * @self: the splash page manager
 *
 * Gets a pointer to the splash page widget. See #EosSplashPageManager:splash-page
 * for more information.
 *
 * Returns: (transfer none): the page #GtkWidget, or %NULL if the splash page
 * has not been set.
 */
GtkWidget *
eos_splash_page_manager_get_splash_page (EosSplashPageManager *self)
{
  g_return_val_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self), NULL);
  
  return self->priv->splash_page;
}

/**
 * eos_splash_page_manager_set_splash_page:
 * @self: the splash page manager
 * @page: the splash page widget
 *
 * Sets the widget for the splash page. See #EosSplashPageManager:splash-page
 * for more information.
 */
void
eos_splash_page_manager_set_splash_page (EosSplashPageManager *self,
                                         GtkWidget      *page)
{
  g_return_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self));
  g_return_if_fail (page == NULL || GTK_IS_WIDGET (page));
  g_return_if_fail (page == NULL || gtk_widget_get_parent (page) == NULL);


  if (self->priv->splash_page != page)
    {
      if (self->priv->splash_page != NULL)
          gtk_container_remove (GTK_CONTAINER (self), self->priv->splash_page);
      if (page != NULL)
        {
          gtk_container_add (GTK_CONTAINER (self), page);
          if (!self->priv->main_page_shown)
              eos_page_manager_set_visible_page (EOS_PAGE_MANAGER (self), page);
        }
      self->priv->splash_page = page;
      g_object_notify( G_OBJECT (self), "splash-page");
    }
}


/**
 * eos_splash_page_manager_get_main_page:
 * @self: the splash page manager
 *
 * Gets a pointer to the main page widget. See #EosSplashPageManager:main-page
 * for more information.
 *
 * Returns: (transfer none): the page #GtkWidget, or %NULL if the main page
 * has not been set.
 */
GtkWidget *
eos_splash_page_manager_get_main_page (EosSplashPageManager *self)
{
  g_return_val_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self), NULL);
  
  return self->priv->main_page;
}

/**
 * eos_splash_page_manager_set_main_page:
 * @self: the splash page manager
 * @page: the main page widget
 *
 * Sets the widget for the main page. See #EosSplashPageManager:main-page for
 * more information.
 */
void
eos_splash_page_manager_set_main_page (EosSplashPageManager *self,
                                       GtkWidget      *page)
{
  g_return_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self));
  g_return_if_fail (page == NULL || GTK_IS_WIDGET (page));
  g_return_if_fail (page == NULL || gtk_widget_get_parent (page) == NULL);

  if (self->priv->main_page != page)
    {
      if (self->priv->main_page != NULL)
          gtk_container_remove (GTK_CONTAINER (self), self->priv->main_page);
      if (page != NULL)
          gtk_container_add (GTK_CONTAINER (self), page);
      self->priv->main_page = page;
      g_object_notify( G_OBJECT (self), "main-page");
    }
}

/**
 * eos_splash_page_manager_show_main_page:
 * @self: the splash page manager
 *
 * Shows the main page of the splash page manager.
 */
void
eos_splash_page_manager_show_main_page (EosSplashPageManager *self)
{
  g_return_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self));
  if (self->priv->main_page == NULL)
    {
      g_critical ("Main page is null, have you set it?");
      return;
    }
  self->priv->main_page_shown = TRUE;
  eos_page_manager_set_visible_page (EOS_PAGE_MANAGER (self), self->priv->main_page);
}

/**
 * eos_splash_page_manager_show_splash_page:
 * @self: the splash page manager
 *
 * Shows the splash page. This page is shown first by default, so this only
 * has effect if you want to "reshow" the page after calling
 * eos_splash_page_manager_show_main_page().
 */
void
eos_splash_page_manager_show_splash_page (EosSplashPageManager *self)
{
  g_return_if_fail (EOS_IS_SPLASH_PAGE_MANAGER (self));
  if (self->priv->splash_page == NULL)
    {
      g_critical ("Splash page is null, have you set it?");
      return;
    }
  self->priv->main_page_shown = FALSE;
  eos_page_manager_set_visible_page (EOS_PAGE_MANAGER (self), self->priv->splash_page);
}
