/* Copyright 2013-2016 Endless Mobile, Inc. */

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

/**
 * EosPageManager:
 *
 * This structure contains no public members.
 */
struct _EosPageManager
{
  GtkStack parent;
};

struct _EosPageManagerClass
{
  GtkStackClass parent_class;

  /* For further expansion */
  gpointer _padding[8];
};

EOS_SDK_AVAILABLE_IN_0_0
GType        eos_page_manager_get_type                            (void) G_GNUC_CONST;

EOS_SDK_AVAILABLE_IN_0_0
GtkWidget   *eos_page_manager_new                                 (void);

EOS_SDK_AVAILABLE_IN_0_0
GtkWidget   *eos_page_manager_get_page_left_topbar_widget         (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void        eos_page_manager_set_page_left_topbar_widget          (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   GtkWidget      *left_topbar_widget);

EOS_SDK_AVAILABLE_IN_0_0
GtkWidget *eos_page_manager_get_page_center_topbar_widget         (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void        eos_page_manager_set_page_center_topbar_widget        (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   GtkWidget      *center_topbar_widget);

EOS_SDK_AVAILABLE_IN_0_0
const gchar *eos_page_manager_get_page_background_uri             (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void         eos_page_manager_set_page_background_uri             (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   const gchar    *background);

EOS_SDK_AVAILABLE_IN_0_0
const gchar *eos_page_manager_get_page_background_size            (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void         eos_page_manager_set_page_background_size            (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   const gchar *size);

EOS_SDK_AVAILABLE_IN_0_0
const gchar *eos_page_manager_get_page_background_position        (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void         eos_page_manager_set_page_background_position        (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   const gchar *position);

EOS_SDK_AVAILABLE_IN_0_0
gboolean     eos_page_manager_get_page_background_repeats         (EosPageManager *self,
                                                                   GtkWidget      *page);

EOS_SDK_AVAILABLE_IN_0_0
void         eos_page_manager_set_page_background_repeats         (EosPageManager *self,
                                                                   GtkWidget      *page,
                                                                   gboolean        repeats);

EOS_SDK_AVAILABLE_IN_0_0
void         eos_page_manager_remove_page_by_name                 (EosPageManager *self,
                                                                   const gchar    *name);

G_END_DECLS

#endif /* EOS_PAGE_MANAGER_H */
