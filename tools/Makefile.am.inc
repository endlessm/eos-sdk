# Copyright 2013 Endless Mobile, Inc.

bin_SCRIPTS = \
	tools/eos-application-manifest/eos-application-manifest \
	tools/eos-json-extractor/eos-json-extractor \
	tools/eos-html-extractor \
	$(NULL)

# Use the following script to replace $datadir inside the script, as suggested
# by the Autoconf manual; because $datadir and friends only work inside
# Makefiles. And as long as we're replacing this, go ahead and replace
# $PACKAGE_VERSION as well.
tools_edit = sed \
	-e 's|@libexecdir[@]|$(libexecdir)|g' \
	-e 's|@PACKAGE_VERSION[@]|$(PACKAGE_VERSION)|g' \
	$(NULL)

tools/eos-application-manifest/eos-application-manifest: tools/eos-application-manifest/eos-application-manifest.in Makefile
	$(AM_V_GEN)$(MKDIR_P) tools/eos-application-manifest && \
	rm -f $@ $@.tmp && \
	$(tools_edit) $< >$@.tmp && \
	chmod +x $@.tmp && \
	chmod a-w $@.tmp && \
	mv $@.tmp $@

CLEANFILES += \
	tools/eos-application-manifest/eos-application-manifest \
	tools/eos-json-extractor/eos-json-extractor \
	$(NULL)
EXTRA_DIST += \
	tools/eos-application-manifest/eos-application-manifest.in \
	tools/eos-json-extractor/eos-json-extractor.in \
	tools/eos-html-extractor \
	$(NULL)

commandsdir = $(libexecdir)/eos-application-manifest/commands
dist_commands_DATA = \
	tools/eos-application-manifest/commands/help.js \
	tools/eos-application-manifest/commands/version.js \
	tools/eos-application-manifest/commands/init.js \
	$(NULL)

EXTRA_DIST += $(tools_test_modules)

bin_PROGRAMS = \
	eos-profile \
	$(NULL)

eos_profile_SOURCES = \
	tools/eos-profile-tool/eos-profile-cmds.h \
	tools/eos-profile-tool/eos-profile-cmd-convert.c \
	tools/eos-profile-tool/eos-profile-cmd-diff.c \
	tools/eos-profile-tool/eos-profile-cmd-help.c \
	tools/eos-profile-tool/eos-profile-cmd-show.c \
	tools/eos-profile-tool/eos-profile-main.c \
	tools/eos-profile-tool/eos-profile-utils.c \
	tools/eos-profile-tool/eos-profile-utils.h \
	endless/gvdb/gvdb-reader.c \
	$(NULL)

eos_profile_CPPFLAGS = \
	@EOS_SDK_CFLAGS@ \
	-DCOMPILING_EOS_SDK \
	-DG_LOG_DOMAIN=\"EosProfile\" \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/endless/ \
	-I$(top_builddir)/endless/ \
	-I$(top_srcdir)/endless/gvdb \
	-I$(top_srcdir)/tools/eos-profile-tool \
	$(NULL)

eos_profile_CFLAGS = $(AM_CFLAGS)
eos_profile_LDADD = -lm @EOS_SDK_LIBS@
