if ENABLE_JS_DOC

build_naturaldocs_verbose = $(build_naturaldocs_verbose_@AM_V@)
build_naturaldocs_verbose_ = $(build_naturaldocs_verbose_@AM_DEFAULT_V@)
build_naturaldocs_verbose_0 = @echo '  DOC    Building WebHelper docs';

docs/reference/webhelper/doc-build.stamp: webhelper/webhelper.js docs/reference/webhelper/eos.css
	$(build_naturaldocs_verbose)$(MKDIR_P) $(@D)/html && \
	naturaldocs -i $(<D) -o HTML $(@D)/html -p $(@D) -s Default eos -hl All

all-local: docs/reference/webhelper/doc-build.stamp

webhelper_extra_clean_files = \
	docs/reference/webhelper/Data/*.nd \
	docs/reference/webhelper/Menu.txt \
	docs/reference/webhelper/Topics.txt \
	docs/reference/webhelper/Languages.txt \
	$(NULL)
EXTRA_DIST += docs/reference/webhelper/eos.css
DISTCLEANFILES += \
	$(webhelper_extra_clean_files) \
	$(NULL)

distclean-local:
	@rm -rf docs/reference/webhelper/html

dist-hook::
	@mkdir -p $(distdir)/docs/reference/webhelper/html
	@cp -r docs/reference/webhelper/html/* $(distdir)/docs/reference/webhelper/html

endif
