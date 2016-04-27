/* Copyright (C) 2013-2016 Endless Mobile, Inc. */

#ifndef EOS_FLEXY_GRID_PRIVATE_H
#define EOS_FLEXY_GRID_PRIVATE_H

#include "eosflexygrid.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL
void            eos_flexy_grid_cell_set_iter            (EosFlexyGridCell *cell,
                                                         GSequenceIter    *iter);
G_GNUC_INTERNAL
GSequenceIter * eos_flexy_grid_cell_get_iter            (EosFlexyGridCell *cell);
G_GNUC_INTERNAL
void            eos_flexy_grid_cell_set_selected        (EosFlexyGridCell *cell,
                                                         gboolean          selected);

G_END_DECLS

#endif /* EOS_FLEXY_GRID_PRIVATE_H */
