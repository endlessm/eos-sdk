if ENABLE_JS_DOC

# This is for moving eos.css into the NaturalDocs project dir, which is in the
# build tree rather than the source tree.
docs/reference/webhelper/eos.css: docs/reference/webhelper/eos.css.in
	$(AM_V_GEN)$(MKDIR_P) $(@D)
	cp $< $@

docs/reference/webhelper/html/index.html: webhelper/webhelper.js docs/reference/webhelper/eos.css
	$(MKDIR_P) $(@D)
	naturaldocs -i $(<D) -o HTML $(@D) -p $(@D)/.. -s Default eos -hl All

all-local: docs/reference/webhelper/html/index.html

webhelper_docs_files = \
	docs/reference/webhelper/html/index.html \
	docs/reference/webhelper/html/files/webhelper-js.html \
	docs/reference/webhelper/html/index/Classes.html \
	docs/reference/webhelper/html/index/Functions.html \
	docs/reference/webhelper/html/index/General.html \
	docs/reference/webhelper/html/index/Properties.html \
	docs/reference/webhelper/html/javascript/main.js \
	docs/reference/webhelper/html/javascript/prettify.js \
	docs/reference/webhelper/html/javascript/searchdata.js \
	docs/reference/webhelper/html/search/*.html \
	docs/reference/webhelper/html/styles/*.css \
	$(NULL)
webhelper_extra_clean_files = \
	docs/reference/webhelper/Data/*.nd \
	docs/reference/webhelper/Menu.txt \
	docs/reference/webhelper/Topics.txt \
	docs/reference/webhelper/Languages.txt \
	docs/reference/webhelper/eos.css \
	$(NULL)
EXTRA_DIST += \
	$(webhelper_docs_files) \
	docs/reference/webhelper/eos.css.in \
	$(NULL)
CLEANFILES += \
	$(webhelper_docs_files) \
	$(webhelper_extra_clean_files) \
	$(NULL)

endif
