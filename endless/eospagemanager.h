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
GType        eos_page_manager_get_type              (void) G_GNUC_CONST;

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_page_manager_new                   (void);

EOS_SDK_ALL_API_VERSIONS
GtkWidget   *eos_page_manager_get_visible_page      (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_visible_page      (EosPageManager *self,
                                                     GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
const gchar *eos_page_manager_get_visible_page_name (EosPageManager *self);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_visible_page_name (EosPageManager *self,
                                                     const gchar    *page_name);

EOS_SDK_ALL_API_VERSIONS
const gchar *eos_page_manager_get_page_name         (EosPageManager *self,
                                                     GtkWidget      *page);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_set_page_name         (EosPageManager *self,
                                                     GtkWidget      *page,
                                                     const gchar    *name);

EOS_SDK_ALL_API_VERSIONS
void         eos_page_manager_remove_page_by_name   (EosPageManager *self,
                                                     const gchar    *name);

G_END_DECLS

#endif /* EOS_PAGE_MANAGER_H */
