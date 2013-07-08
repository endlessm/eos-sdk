# Copyright 2013 Endless Mobile, Inc.

bin_SCRIPTS = \
	tools/eos-run-test \
	tools/eos-application-manifest/eos-application-manifest
	$(NULL)

# Use the following script to replace $datadir inside the script, as suggested
# by the Autoconf manual; because $datadir and friends only work inside
# Makefiles. And as long as we're replacing this, go ahead and replace
# $PACKAGE_VERSION as well.
tools_edit = sed \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@PACKAGE_VERSION[@]|$(PACKAGE_VERSION)|g' \
	$(NULL)
tools/eos-application-manifest/eos-application-manifest: tools/eos-application-manifest/eos-application-manifest.in Makefile
	$(AM_V_GEN)$(MKDIR_P) tools/eos-application-manifest && \
	rm -f $@ $@.tmp && \
	$(tools_edit) $< >$@.tmp && \
	chmod +x $@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@

CLEANFILES += tools/eos-application-manifest/eos-application-manifest
EXTRA_DIST += tools/eos-application-manifest/eos-application-manifest.in

commandsdir = $(datarootdir)/eos-application-manifest/commands
dist_commands_DATA = \
	tools/eos-application-manifest/commands/help.js \
	tools/eos-application-manifest/commands/version.js \
	$(NULL)
