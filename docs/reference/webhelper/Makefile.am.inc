if ENABLE_JS_DOC

# This is for moving eos.css into the NaturalDocs project dir, which is in the
# build tree rather than the source tree.
docs/reference/webhelper/eos.css: docs/reference/webhelper/eos.css.in
	$(AM_V_GEN)$(MKDIR_P) $(@D)
	cp $< $@

build_naturaldocs_verbose = $(build_naturaldocs_verbose_@AM_V@)
build_naturaldocs_verbose_ = $(build_naturaldocs_verbose_@AM_DEFAULT_V@)
build_naturaldocs_verbose_0 = @echo '  DOC    Building WebHelper docs';

docs/reference/webhelper/html/index.html: webhelper/webhelper.js docs/reference/webhelper/eos.css
	$(build_naturaldocs_verbose)$(MKDIR_P) $(@D) && \
	naturaldocs -i $(<D) -o HTML $(@D) -p $(@D)/.. -s Default eos -hl All

all-local: docs/reference/webhelper/html/index.html

webhelper_extra_clean_files = \
	docs/reference/webhelper/Data/*.nd \
	docs/reference/webhelper/Menu.txt \
	docs/reference/webhelper/Topics.txt \
	docs/reference/webhelper/Languages.txt \
	$(NULL)
EXTRA_DIST += docs/reference/webhelper/eos.css.in
CLEANFILES += docs/reference/webhelper/eos.css
DISTCLEANFILES += \
	$(webhelper_extra_clean_files) \
	$(NULL)

distclean-local:
	@rm -rf docs/reference/webhelper/html

dist-hook:
	@mkdir -p $(distdir)/docs/reference/webhelper/html
	@cp -r docs/reference/webhelper/html/* $(distdir)/docs/reference/webhelper/html

endif
