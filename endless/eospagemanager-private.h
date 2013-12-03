/* Copyright 2013 Endless Mobile, Inc. */

#ifndef EOS_PAGE_MANAGER_PRIVATE_H
#define EOS_PAGE_MANAGER_PRIVATE_H

#include "eostypes.h"

#include "eospagemanager.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

GtkStackTransitionType eos_page_manager_get_gtk_stack_transition_type (EosPageManager *self);

G_END_DECLS

#endif /* EOS_PAGE_MANAGER_PRIVATE_H */
