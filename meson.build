project('eos-sdk', 'c',
  version: '0.5.0',
  meson_version: '>= 0.43.0',
  default_options: [
    'warning_level=1',
    'c_std=c99',
    'buildtype=debugoptimized',
  ],
)

cc = meson.get_compiler('c')

# Versions
version = meson.project_version().split('.')
sdk_major_version = version[0].to_int()
sdk_minor_version = version[1].to_int()
sdk_micro_version = version[2].to_int()

api_name = 'endless-@0@'.format(sdk_major_version)

# Keep compatibility with libtool versioning
interface_age = sdk_minor_version.is_odd() ? 0 : sdk_micro_version
current = 100 * sdk_minor_version + sdk_micro_version - interface_age
revision = interface_age
age = current
soversion = 0
libversion = '@0@.@1@.@2@'.format(current, revision, age)

compiler_flags = [
  '-Wcast-align',
  '-Wuninitialized',
  '-Wno-strict-aliasing',
  '-Werror=pointer-arith',
  '-Werror=missing-declarations',
  '-Werror=redundant-decls',
  '-Werror=empty-body',
  '-Werror=format=2',
]

strict_flags = cc.get_supported_arguments(compiler_flags)

# Paths
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
datadir = join_paths(prefix, get_option('datadir'))
includedir = join_paths(prefix, get_option('includedir'))
localedir = join_paths(prefix, get_option('localedir'))
sysconfdir = get_option('sysconfdir')

# Requirements
glib_req = '>= 2.38'
gtk_req = '>= 3.20'
json_glib_req = '>= 1.0'

sdk_required = [
  'glib-2.0 ' + glib_req,
  'gobject-2.0',
  'gio-2.0',
  'gtk+-3.0 ' + gtk_req,
  'json-glib-1.0 ' + json_glib_req,
]

sdk_required_private = []

glib_dep = dependency('glib-2.0', version: glib_req)
gio_dep = dependency('gio-2.0')
gtk_dep = dependency('gtk+-3.0', version: gtk_req)
json_glib_dep = dependency('json-glib-1.0', version: json_glib_req)

enable_metrics = get_option('metrics')
if enable_metrics
  metrics_dep = dependency('eos-metrics')
  sdk_required_private += ['eos-metrics-0']
else
  metrics_dep = []
endif

sdk_deps = [
  glib_dep,
  gio_dep,
  gtk_dep,
  json_glib_dep,
  metrics_dep,
]

gir_doc_tool = find_program('g-ir-doc-tool', required: false)
yelp_build = find_program('yelp-build', required: false)
if get_option('gir_doc')
  if not gir_doc_tool.found()
    error('GIR documentation needs g-ir-doc-tool')
  endif
  if not yelp_build.found()
    error('GIR documentation needs yelp-build')
  endif
endif

dbus_run_session = find_program('dbus-run-session', required: false)

gnome = import('gnome')
pkgconf = import('pkgconfig')
i18n = import('i18n')

root_inc = include_directories('.')

subdir('endless')
subdir('licenses')
subdir('overrides')
#subdir('test')
#subdir('tools')
#subdir('docs')
#subdir('po')
subdir('m4')

pkgconf.generate(
  name: 'Open Endless SDK',
  version: meson.project_version(),
  description: 'Software development kit for application for the developing world',
  url: 'http://endlessm.com',
  libraries: libendless,
  filebase: api_name,
  subdirs: api_name,
  requires: sdk_required,
  requires_private: sdk_required_private,
)
