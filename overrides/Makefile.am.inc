# Copyright 2013 Endless Mobile, Inc.

# # # GJS OVERRIDES # # #

gjsdir = ${datadir}/gjs-1.0

# Put override files here for custom js API outside of normal introspection
nobase_dist_gjs_DATA = \
    overrides/Endless.js \
    overrides/endless_private/asset_button.js \
    overrides/endless_private/injectable_webview.js \
    overrides/endless_private/connection_test.js \
    overrides/endless_private/search_box.js \
    overrides/endless_private/topbar_nav_button.js \
    $(NULL)
