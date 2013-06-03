/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_SPLASH_PAGE_MANAGER_H
#define EOS_SPLASH_PAGE_MANAGER_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

#include "eospagemanager.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_SPLASH_PAGE_MANAGER eos_splash_page_manager_get_type()

#define EOS_SPLASH_PAGE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_SPLASH_PAGE_MANAGER, EosSplashPageManager))

#define EOS_SPLASH_PAGE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_SPLASH_PAGE_MANAGER, EosSplashPageManagerClass))

#define EOS_IS_SPLASH_PAGE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_SPLASH_PAGE_MANAGER))

#define EOS_IS_SPLASH_PAGE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_SPLASH_PAGE_MANAGER))

#define EOS_SPLASH_PAGE_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_SPLASH_PAGE_MANAGER, EosSplashPageManagerClass))

typedef struct _EosSplashPageManager EosSplashPageManager;
typedef struct _EosSplashPageManagerClass EosSplashPageManagerClass;
typedef struct _EosSplashPageManagerPrivate EosSplashPageManagerPrivate;

/**
 * EosSplashPageManager:
 *
 * This structure contains no public members.
 */
struct _EosSplashPageManager
{
  EosPageManager parent;

  EosSplashPageManagerPrivate *priv;
};

struct _EosSplashPageManagerClass
{
  EosPageManagerClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_ALL_API_VERSIONS
GType        eos_splash_page_manager_get_type         (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_splash_page_manager_new              (void);

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_splash_page_manager_get_splash_page  (EosSplashPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_splash_page_manager_set_splash_page  (EosSplashPageManager *self,
                                                       GtkWidget            *page);

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_splash_page_manager_get_main_page    (EosSplashPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_splash_page_manager_set_main_page    (EosSplashPageManager *self,
                                                       GtkWidget            *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_splash_page_manager_show_main_page   (EosSplashPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_splash_page_manager_show_splash_page (EosSplashPageManager *self);

G_END_DECLS

#endif /* EOS_SPLASH_PAGE_MANAGER_H */
