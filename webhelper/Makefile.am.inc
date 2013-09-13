# Copyright 2013 Endless Mobile, Inc.

# # # INSTALL RULES # # #

webhelper_sources = webhelper/webhelper.js

webhelperdir = $(gjsmodulesdir)
dist_webhelper_DATA = \
    $(webhelper_sources) \
    $(NULL)
