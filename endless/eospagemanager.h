/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_PAGE_MANAGER_H
#define EOS_PAGE_MANAGER_H

#if !(defined(_EOS_SDK_INSIDE_ENDLESS_H) || defined(COMPILING_EOS_SDK))
#error "Please do not include this header file directly."
#endif

#include "eostypes.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EOS_TYPE_PAGE_MANAGER eos_page_manager_get_type()

#define EOS_PAGE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  EOS_TYPE_PAGE_MANAGER, EosPageManager))

#define EOS_PAGE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  EOS_TYPE_PAGE_MANAGER, EosPageManagerClass))

#define EOS_IS_PAGE_MANAGER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  EOS_TYPE_PAGE_MANAGER))

#define EOS_IS_PAGE_MANAGER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  EOS_TYPE_PAGE_MANAGER))

#define EOS_PAGE_MANAGER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  EOS_TYPE_PAGE_MANAGER, EosPageManagerClass))

typedef struct _EosPageManager EosPageManager;
typedef struct _EosPageManagerClass EosPageManagerClass;
typedef struct _EosPageManagerPrivate EosPageManagerPrivate;

#define EOS_TYPE_PAGE_MANAGER_TRANSITION_TYPE (eos_page_manager_transition_type_get_type ())

/**
 * EosPageManagerTransitionType:
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE: No animation, regardless of duration.
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE: Pages will not move, but fade into one another.
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_RIGHT: The old page will slide off to the right.
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_LEFT: The old page will slide off to the left.
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_UP: The old page will slide up the screen.
 * @EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_DOWN: The old page will slide down the screen.
 *
 * Enum values to specify the type of animation to use when transitioning
 * between pages.
 **/
typedef enum {
  EOS_PAGE_MANAGER_TRANSITION_TYPE_NONE,
  EOS_PAGE_MANAGER_TRANSITION_TYPE_CROSSFADE,
  EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_RIGHT,
  EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_LEFT,
  EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_UP,
  EOS_PAGE_MANAGER_TRANSITION_TYPE_SLIDE_DOWN
} EosPageManagerTransitionType;

/**
 * EosPageManager:
 *
 * This structure contains no public members.
 */
struct _EosPageManager
{
  GtkContainer parent;

  EosPageManagerPrivate *priv;
};

struct _EosPageManagerClass
{
  GtkContainerClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_ALL_API_VERSIONS
GType        eos_page_manager_get_type                            (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GType        eos_page_manager_transition_type_get_type            (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_page_manager_new                                 (void);

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_page_manager_get_visible_page                    (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_visible_page                    (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
const gchar *eos_page_manager_get_visible_page_name               (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_visible_page_name               (EosPageManager *self,
                                                                   const gchar    *page_name);

EOS_SDK_ALL_API_VERSIONS
const gchar *eos_page_manager_get_page_name                       (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_page_name                       (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   const gchar    *name);

EOS_SDK_ALL_API_VERSIONS
gboolean     eos_page_manager_get_page_actions                    (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_page_actions                    (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   gboolean        actions_visible);

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_page_manager_get_page_custom_toolbox_widget      (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_page_custom_toolbox_widget      (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   GtkWidget      *custom_toolbox_widget);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_transition_duration             (EosPageManager *self,
                                                                   guint           duration);

EOS_SDK_ALL_API_VERSIONS
guint        eos_page_manager_get_transition_duration             (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_transition_type                 (EosPageManager *self,
                                                                   EosPageManagerTransitionType  transition);

EOS_SDK_ALL_API_VERSIONS
EosPageManagerTransitionType eos_page_manager_get_transition_type (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
const gchar *eos_page_manager_get_page_background_uri             (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_page_background_uri             (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   const gchar    *background);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_remove_page_by_name                 (EosPageManager *self,
                                                                   const gchar    *name);

G_END_DECLS

#endif /* EOS_PAGE_MANAGER_H */
