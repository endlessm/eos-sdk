# Copyright 2013 Endless Mobile, Inc.

# # # CONFIGURE SCRIPT # # #

# Create a config.js file with the installed directory locations. Use the
# following sed script to replace $datadir inside the script, as suggested by
# the Autoconf manual; because $datadir and friends only work inside Makefiles.
edit = sed \
	-e 's|@datadir[@]|$(datadir)|g' \
	$(NULL)
wikipedia/config.js: wikipedia/config.js.in Makefile
	$(AM_V_GEN)mkdir -p wikipedia && \
	rm -f $@ $@.tmp && \
	$(edit) $< >$@.tmp && \
	chmod +x $@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@
CLEANFILES += wikipedia/config.js
EXTRA_DIST += wikipedia/config.js.in

# # # INSTALL RULES # # #

js_sources = \
	wikipedia/ArticleList.js \
	wikipedia/widgets/BackButton.js \
	wikipedia/widgets/BoxWithBg.js \
	wikipedia/widgets/FixedSizeTextView.js \
	wikipedia/EndlessWikipedia.js \
	wikipedia/PrebuiltArticlesPage.js \
	wikipedia/PrebuiltCategoryPage.js \
	wikipedia/PrebuiltFrontPage.js \
	wikipedia/PrebuiltWikipediaApplication.js \
	wikipedia/widgets/ListTextButton.js \
	wikipedia/config.js \
	wikipedia/widgets/scaled_image.js \
	wikipedia/utils.js \
	wikipedia/WikipediaApplication.js \
	wikipedia/models/article_model.js \
	wikipedia/models/category_model.js \
	wikipedia/models/domain_wiki_model.js \
	wikipedia/models/utils/json_utils.js \
	wikipedia/models/utils/locale_utils.js \
	wikipedia/presenters/domain_wiki_presenter.js \
	wikipedia/widgets/category_button.js \
	wikipedia/widgets/category_layout_manager.js \
	wikipedia/widgets/category_selector_view.js \
	wikipedia/views/domain_wiki_view.js \
	wikipedia/WikipediaWebView.js \
	$(NULL)

wikipediadir = $(gjsmodulesdir)
nobase_dist_wikipedia_DATA = \
	$(js_sources) \
	$(NULL)
