dnl Copyright 2013-2015 Endless Mobile, Inc.
dnl
dnl Macro to define translation rules for HTML files
dnl Usage: EOS_DEFINE_I18N_RULES (no arguments)
dnl
dnl Usage in Makefile:
dnl HTML_I18N_RULES:
dnl Just include @HTML_I18N_RULES@ and then for each file.html that has
dnl internationalizable values (tags with name="translatable"), put
dnl file.html.dummy.c in your po/POTFILES.in file. Also make sure the dummy file
dnl gets made by putting it in all-am or noinst_DATA.
dnl JSON_I18N_RULES:
dnl Same as HTML_I18N_RULES, but scans file.json for keys ending with an
dnl underscore, and puts those keys' values in file.json.dummy.c.
dnl
dnl Don't forget to add --directory=$(top_builddir) to XGETTEXT_OPTIONS in
dnl po/Makevars, since xgettext by default only looks for the files listed in
dnl POTFILES.in in the source tree, not in the build tree.

AC_DEFUN([EOS_DEFINE_I18N_RULES],
[
 m4_pattern_allow([AM_V_GEN])  dnl Otherwise the variable is not allowed
 AC_REQUIRE([AC_PROG_MKDIR_P])
 HTML_I18N_RULES='
%.html.dummy.c: %.html
	$(AM_V_GEN)$(MKDIR_P) [$](@D) && \
	eos-html-extractor $< $(top_srcdir) >[$]@
'

 AC_SUBST([HTML_I18N_RULES])
 AM_SUBST_NOTMAKE([HTML_I18N_RULES])

 JSON_I18N_RULES='
%.json.dummy.c: %.json
	$(AM_V_GEN)$(MKDIR_P) [$](@D) && \
	eos-json-extractor $< $(top_srcdir) >[$]@
'

 AC_SUBST([JSON_I18N_RULES])
 AM_SUBST_NOTMAKE([JSON_I18N_RULES])
])
