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
	wikipedia/EndlessWikipedia.js \
	wikipedia/PrebuiltArticlesPage.js \
	wikipedia/PrebuiltCategoryPage.js \
	wikipedia/PrebuiltFrontPage.js \
	wikipedia/PrebuiltWikipediaApplication.js \
	wikipedia/widgets/SideBarButton.js \
	wikipedia/widgets/TextButton.js \
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
	wikipedia/widgets/title_label_view.js \
	wikipedia/WikipediaWebView.js \
	$(NULL)

parafernalia_sources = \
	wikipedia/views/css/first_load.css \
	wikipedia/views/css/main.css \
	wikipedia/views/css/nolinks.css \
	wikipedia/views/css/second_load.css \
	wikipedia/views/css/wikipedia.css \
	wikipedia/views/fonts/BentonSans-Bold.eot \
	wikipedia/views/fonts/BentonSans-Bold.svg \
	wikipedia/views/fonts/BentonSans-Bold.ttf \
	wikipedia/views/fonts/BentonSans-Bold.woff \
	wikipedia/views/fonts/BentonSans-Book.eot \
	wikipedia/views/fonts/BentonSans-Book.svg \
	wikipedia/views/fonts/BentonSans-Book.ttf \
	wikipedia/views/fonts/BentonSans-Book.woff \
	wikipedia/views/fonts/BentonSans-Medium.eot \
	wikipedia/views/fonts/BentonSans-Medium.svg \
	wikipedia/views/fonts/BentonSans-Medium.ttf \
	wikipedia/views/fonts/BentonSans-Medium.woff \
	wikipedia/views/fonts/BentonSans-Regular.eot \
	wikipedia/views/fonts/BentonSans-Regular.svg \
	wikipedia/views/fonts/BentonSans-Regular.ttf \
	wikipedia/views/fonts/BentonSans-Regular.woff \
	wikipedia/views/fonts/BentonSans-Thin.eot \
	wikipedia/views/fonts/BentonSans-Thin.svg \
	wikipedia/views/fonts/BentonSans-Thin.ttf \
	wikipedia/views/fonts/BentonSans-Thin.woff \
	wikipedia/views/fonts/LinLibertine_R.eot \
	wikipedia/views/fonts/LinLibertine_R.svg \
	wikipedia/views/fonts/LinLibertine_R.ttf \
	wikipedia/views/fonts/LinLibertine_R.woff \
	wikipedia/views/fonts/MinionPro-Regular.eot \
	wikipedia/views/fonts/MinionPro-Regular.svg \
	wikipedia/views/fonts/MinionPro-Regular.ttf \
	wikipedia/views/fonts/MinionPro-Regular.woff \
	wikipedia/views/images/bg-main-content.png \
	wikipedia/views/images/hr-separate.png \
	wikipedia/views/images/index_list_arrow-down.svg \
	wikipedia/views/images/title-line.jpg \
	wikipedia/views/js/jquery-min.js \
	wikipedia/views/js/main.js \
	$(NULL)

gjsmodulesdir = $(datadir)/gjs-1.0
nobase_dist_gjsmodules_DATA = \
	$(js_sources) \
	$(parafernalia_sources) \
	$(NULL)
