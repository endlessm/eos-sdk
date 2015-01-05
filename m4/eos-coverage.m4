dnl Copyright 2014 Endless Mobile, Inc.
dnl
dnl Macros to check for code coverage support

# $1: Coverage output directory
AC_DEFUN_ONCE([EOS_C_COVERAGE], [
  AC_PATH_PROG([EOS_COVERAGE_LCOV], [lcov], [notfound])
  AC_ARG_VAR([EOS_COVERAGE_LCOV], [Path to lcov])

  # If we have LCOV, then we can do coverage counting on the C side
  # at least
  AS_IF([test "x$EOS_COVERAGE_LCOV" = "xnotfound"], [
    EOS_HAVE_C_COVERAGE=no
  ], [
    # Turn on the coverage generating flags in GCC
    EOS_COVERAGE_COMPILER_FLAGS="-g -O0 -coverage"
    EOS_C_COVERAGE_LDFLAGS="-lgcov"
    AS_COMPILER_FLAGS([EOS_C_COVERAGE_CFLAGS], [$EOS_COVERAGE_COMPILER_FLAGS])

    EOS_HAVE_C_COVERAGE=yes
    EOS_C_COVERAGE_OUTPUT_PATH='$1/output/c'
  ])
])

# $1: Coverage output directory
AC_DEFUN_ONCE([EOS_JS_COVERAGE], [
  PKG_CHECK_MODULES([GJS_WITH_COVERAGE], [gjs-1.0 >= 1.40.0], [
    EOS_HAVE_JS_COVERAGE=yes
    EOS_JS_COVERAGE_OUTPUT_PATH='$1/output/js'
  ], [
    EOS_HAVE_JS_COVERAGE=no
  ])
])

# $1: Coverage output directory
AC_DEFUN_ONCE([EOS_COVERAGE_REPORT], [
  # Enable the --enable-coverage switch, although it'll only be effective
  # if we can actually do coverage reports
  AC_ARG_ENABLE([coverage],
    [AS_HELP_STRING([--enable-coverage],
       [Generate code coverage statistics when running tests @<:@default=no@:>@])
  ])

  AS_IF([test "x$EOS_HAVE_C_COVERAGE" = "xyes" || test "x$EOS_HAVE_JS_COVERAGE" = "xyes"], [
    AC_PATH_PROG([EOS_COVERAGE_GENHTML], [genhtml], [notfound])
    AC_ARG_VAR([EOS_COVERAGE_GENHTML], [Path to genhtml])
    AS_IF([test "x$EOS_COVERAGE_GENHTML" = "xnotfound"], [
      EOS_HAVE_GENHTML=no
    ], [
      EOS_HAVE_GENHTML=yes
    ])

    AC_PATH_PROG([EOS_COVERAGE_LCOV_RESULT_MERGER], [lcov-result-merger], [notfound])
    AC_ARG_VAR([EOS_COVERAGE_LCOV_RESULT_MERGER], [Path to lcov-result-merger])

    AS_IF([test "x$EOS_COVERAGE_LCOV_RESULT_MERGER" = "xnotfound"], [
      EOS_HAVE_COBERTURA=no
    ], [
      EOS_HAVE_COBERTURA=yes
    ])

    # We have coverage reporting as long as we have either cobertura or
    # genhtml support
    AS_IF([test "x$EOS_HAVE_COBERTURA" = "xyes" || test "x$EOS_HAVE_GENHTML" = "xyes" ], [
      EOS_HAVE_COVERAGE_REPORT=yes
      EOS_COVERAGE_REPORT_PATH='$1/report'
    ], [
      EOS_HAVE_COVERAGE_REPORT=no
    ])
  ], [
    EOS_HAVE_COVERAGE_REPORT=no
  ])

  AS_IF([test "x$enable_coverage" = "xyes" && test "x$EOS_HAVE_COVERAGE_REPORT" = "xyes"], [
    AS_IF([test "x$EOS_HAVE_C_COVERAGE" = "xyes"], [
      dnl Strip leading spaces
      EOS_C_COVERAGE_CFLAGS=${EOS_C_COVERAGE_CFLAGS#*  }
      EOS_ENABLE_C_COVERAGE=yes
    ])
    EOS_ENABLE_JS_COVERAGE=$EOS_HAVE_JS_COVERAGE
    EOS_ENABLE_COVERAGE=yes
  ])

  # Now that we've figured out if we have coverage reports, build the rules
  AS_IF([test "x$EOS_ENABLE_COVERAGE" = "xyes"], [
    EOS_COVERAGE_RULES_HEADER='

# Copyright 2014 Endless Mobile, Inc.
#
# This makefile should be used in conjunction with eos-coverage.m4 and
# Makefile-jasmine.am.inc from eos-jasmine.
#
# If you wish to use the JavaScript coverage function, Makefile-jasmine.am.inc
# should be included before this Makefile.
#
# Add clean-coverage to the clean-local target in your Makefile to get the clean
# rules for the coverage data.
#
# Variables that affect the operation of this Makefile:
#
#  - EOS_JS_COVERAGE_FILES: The list of JavaScript files to be included
#    in the JavaScript coverage report. This must be set before including
#    this Makefile if EOS_ENABLE_JS_COVERAGE was enabled.
#
# If EOS_ENABLE_C_COVERAGE was enabled and coverage reporting for the
# project has been enabled on the commandline, then this Makefile will
# add -coverage to CFLAGS and LDFLAGS and also create a "eos-c-coverage"
# target which collects line and funciton hit counter data and places
# it in $coverage_directory/output/c/coverage.lcov
#
# If EOS_ENABLE_JS_COVERAGE was enabled and coverage reporting for this
# project has been enabled on the commandline, then this Makefile will
# add the coverage generating switch to AM_JS_LOG_FLAGS for all files
# specified in EOS_JS_COVERAGE_FILES.

# Internal variable to track the coverage accumulated counter files.
_eos_coverage_outputs = 
_eos_collect_coverage_targets = 
_eos_clean_coverage_targets =
'

    EOS_COVERAGE_RULES_TARGETS='
# Internal variable for the genhtml coverage report path
_eos_genhtml_coverage_report_path = $(EOS_COVERAGE_REPORT_PATH)/genhtml

# Internal variable for the cobertura coverage report path
_eos_cobertura_coverage_report_path = $(EOS_COVERAGE_REPORT_PATH)/cobertura

# Set up an intermediate eos-collect-coverage target
# which just runs the language specific coverage collection
# targets
eos-collect-coverage: $(_eos_collect_coverage_targets)

# The clean-coverage target runs the language specific
# clean rules and also cleans the generated html reports
clean-coverage: $(_eos_clean_coverage_targets)
	rm -rf $(_eos_genhtml_coverage_report_path)
	rm -rf $(_eos_cobertura_coverage_report_path)
'
  ], [
    EOS_COVERAGE_RULES_TARGETS='
# Define the targets just to print an error if coverage reports are not enabled
eos-collect-coverage:
	@echo "--enable-coverage must be passed to ./configure and lcov/genhtml installed"
	@exit 1

clean-coverage:
	@echo "no coverage data generated, so none to clean"
  '])

  AS_IF([test "x$EOS_HAVE_GENHTML" = "xyes"], [
    EOS_GENHTML_COVERAGE_RULES='
# Check that required variable EOS_COVERAGE_GENHTML is set
$(if $(EOS_COVERAGE_GENHTML),,$(error GenHTML not found, ensure that eos-coverage.m4 was included in configure.ac))

# The "coverage-genhtml" target depends on eos-collect-coverage
# and then runs genhtml on the coverage counters. This is useful
# if you are just looking at coverage data locally.
coverage-genhtml: eos-collect-coverage
	$(EOS_COVERAGE_GENHTML) --legend -o $(_eos_genhtml_coverage_report_path) $(_eos_coverage_outputs)
'
  ], [
    EOS_GENHTML_COVERAGE_RULES='
coverage-genhtml: eos-collect-covearge
	@echo "Cannot generate GenHTML coverage report as genhtml was not found in PATH"
	@exit 1
'])

  AS_IF([test "x$EOS_HAVE_COBERTURA" = "xyes"], [
    EOS_COBERTURA_COVERAGE_RULES='
# The "coverage-cobertura" target depends on eos-collect-coverage
# and then runs lcov_cobertura.py to convert it to a cobertura compatible
# XML file format
_eos_lcov_cobertura_url = "https://raw.githubusercontent.com/eriwen/lcov-to-cobertura-xml/8547b138ef07a2ca64aa9b7ff3556a44bddff128/lcov_cobertura/lcov_cobertura.py"
_eos_lcov_cobertura_script = $(_eos_cobertura_coverage_report_path)/lcov_cobertura.py
_eos_lcov_cobertura_sha1sum = "355f738736e1b610ff2ebc00f331e5db5b25c65a"

# Paths to each stage of the cobertura coverage report
# 1. Collated path
# 2. Merged path
# 3. XML path
_eos_cobertura_collated_path = $(_eos_cobertura_coverage_report_path)/collated.lcov
_eos_cobertura_merged_path = $(_eos_cobertura_coverage_report_path)/merged.lcov
_eos_cobertura_xml_path = $(_eos_cobertura_coverage_report_path)/cobertura.xml

# We need to download lcov_cobertura.py (and check that it is valid) and then
# execute it on our lcov files.
#
# Converting spaces to newlines to pass to lcov_cobertura.py here
# is probably not ideal. We are assuming that the filenames specified in
# _eos_coverage_outputs dont have spaces in them and that there are no
# trailing or leading whitespaces.
coverage-cobertura: eos-collect-coverage
	mkdir -p $(_eos_cobertura_coverage_report_path)
	wget -c -q $(_eos_lcov_cobertura_url) -O $(_eos_lcov_cobertura_script)
	echo > $(_eos_cobertura_collated_path)
	echo $(_eos_coverage_outputs) | sed -e "s/\s\+/\n/g" | xargs -L1 -I {} cat {} >> $(_eos_cobertura_collated_path)
	$(EOS_COVERAGE_LCOV_RESULT_MERGER) $(_eos_cobertura_collated_path) $(_eos_cobertura_merged_path)
	python $(_eos_lcov_cobertura_script) $(_eos_cobertura_merged_path) -o $(_eos_cobertura_xml_path)
'
  ], [
    EOS_COBERTURA_COVERAGE_RULES='
coverage-cobertura: eos-collect-covearge
	@echo "Cannot generate Cobertura coverage report as lcov-result-merger was not found in PATH"
	@exit 1
'])

  AS_IF([test "x$EOS_ENABLE_JS_COVERAGE" = "xyes"], [
    EOS_JS_COVERAGE_RULES='
# First check that all the required variables have been set. This includes:
# - EOS_JS_COVERAGE_FILES
# - AM_JS_LOG_FLAGS

$(if $(EOS_JS_COVERAGE_FILES),,$(error Need to define EOS_JS_COVERAGE_FILES))

# Internal variables for the coverage data output path and file
_eos_js_coverage_data_output_file := $(EOS_JS_COVERAGE_OUTPUT_PATH)/coverage.lcov

# Add _eos_js_coverage_data_output_file to _eos_coverage_outputs
_eos_coverage_outputs += $(_eos_js_coverage_data_output_file)

# Finally add a eos-clean-js-coverage rule which cleans out
# EOS_JS_COVERAGE_OUTPUT_PATH
eos-clean-js-coverage:
	rm -rf $(EOS_JS_COVERAGE_OUTPUT_PATH)

_eos_clean_coverage_targets += eos-clean-js-coverage
'

    # This small fragment collects all the paths and add the --coverage-path prefix
    # to each one, finally adding --coverage-output. This makes the list of flags we
    # will pass to gjs to enable coverage reports.
    EOS_AM_JS_LOG_FLAGS_ADD='$(addprefix --coverage-path=,$(EOS_JS_COVERAGE_FILES)) --coverage-output=$(EOS_JS_COVERAGE_OUTPUT_PATH)'
  ])

  AS_IF([test "x$EOS_ENABLE_C_COVERAGE" = "xyes"], [
    EOS_C_COVERAGE_RULES='
# First check that all the required variables have been set. This includes:
# - EOS_COVERAGE_LCOV
# - EOS_COVERAGE_GENHTML

$(if $(EOS_COVERAGE_LCOV),,$(error LCov not found, ensure that eos-coverage.m4 was included in configure.ac))

# Define internal variables to keep the C coverage counters in
_eos_c_coverage_data_output_file = $(EOS_C_COVERAGE_OUTPUT_PATH)/coverage.lcov
_eos_c_coverage_data_output_tmp_file = $(_eos_c_coverage_data_output_file).tmp

# Add final coverage output file to list of coverage data files
_eos_coverage_outputs += $(_eos_c_coverage_data_output_file)

# Define an eos-c-coverage target to generate the coverage counters
eos-c-coverage:
	test -d $(EOS_C_COVERAGE_OUTPUT_PATH) || mkdir -p $(EOS_C_COVERAGE_OUTPUT_PATH)
	$(EOS_COVERAGE_LCOV) --compat-libtool --capture --directory $(abs_top_builddir) -o $(_eos_c_coverage_data_output_tmp_file)
	$(EOS_COVERAGE_LCOV) --extract $(_eos_c_coverage_data_output_tmp_file) "$(abs_top_srcdir)/*" -o $(_eos_c_coverage_data_output_file)
	rm -rf $(_eos_c_coverage_data_output_tmp_file)

eos-clean-c-coverage:
	find $(abs_top_builddir) -name "*.gcda" -delete
	find $(abs_top_builddir) -name "*.gcno" -delete
	rm -rf $(EOS_C_COVERAGE_OUTPUT_PATH)

_eos_collect_coverage_targets += eos-c-coverage
_eos_clean_coverage_targets += eos-clean-c-coverage
'
])

  EOS_COVERAGE_RULES_FOOTER='
.PHONY: coverage clean-coverage
'

  EOS_COVERAGE_RULES="$EOS_COVERAGE_RULES_HEADER $EOS_GENHTML_COVERAGE_RULES $EOS_COBERTURA_COVERAGE_RULES $EOS_C_COVERAGE_RULES $EOS_JS_COVERAGE_RULES $EOS_COVERAGE_RULES_TARGETS $EOS_COVERAGE_RULES_FOOTER"

  # Substitute these at the top first
  AC_SUBST([EOS_COVERAGE_REPORT_PATH])
  AC_SUBST([EOS_C_COVERAGE_OUTPUT_PATH])
  AC_SUBST([EOS_JS_COVERAGE_OUTPUT_PATH])

  # We only want to define this to use it for full substitution, not as a variable
  AC_SUBST([EOS_COVERAGE_RULES])
  AM_SUBST_NOTMAKE([EOS_COVERAGE_RULES])

  AC_SUBST([EOS_AM_JS_LOG_FLAGS_ADD])
  AC_SUBST(EOS_C_COVERAGE_CFLAGS)
  AC_SUBST(EOS_C_COVERAGE_LDFLAGS)
])
