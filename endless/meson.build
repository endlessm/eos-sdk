resources = gnome.compile_resources('eosresource-private',
  '../data/eos-sdk.gresource.xml',
  source_dir: '../data',
)

eosversion_h = configuration_data()
eosversion_h.set('EOS_SDK_API_VERSION', sdk_major_version)
eosversion_h.set('EOS_SDK_MAJOR_VERSION', sdk_major_version)
eosversion_h.set('EOS_SDK_MINOR_VERSION', sdk_minor_version)
eosversion_h.set('EOS_SDK_MICRO_VERSION', sdk_micro_version)
configure_file(input: 'eosversion.h.in',
  output: 'eosversion.h',
  configuration: eosversion_h,
  install: true,
  install_dir: join_paths(includedir, api_name, 'endless'),
)

installed_headers = [
  'eosapiversion.h',
  'eosapplication.h',
  'eoscustomcontainer.h',
  'eosenums.h',
  'eoslicense.h',
  'eosmacros.h',
  'eospagemanager.h',
  'eostypes.h',
  'eoswindow.h',
  'eosflexygrid.h',
]

sources = [
  'eosapplication.c',
  'eosattribution.c',
  'eoscellrendererpixbuflink.c',
  'eoscellrenderertextlink.c',
  'eoscustomcontainer.c',
  'eoshello.c',
  'eosinit.c',
  'eoslicense.c',
  'eospagemanager.c',
  'eostopbar.c',
  'eosutil.c',
  'eoswindow.c',
  'eosflexygrid.c',
  'eosflexygridcell.c',
]

config_h = configuration_data()
config_h.set_quoted('GETTEXT_PACKAGE', meson.project_name())
configure_file(output: 'config.h', configuration: config_h)

install_headers(installed_headers + ['endless.h'],
  subdir: join_paths(api_name, 'endless'),
)

libendless = library(api_name,
  sources: sources + resources,
  soversion: soversion,
  version: libversion,
  c_args: [
    '-DG_LOG_DOMAIN="EndlessSDK"',
    '-DCOMPILING_EOS_SDK',
    '-DDATADIR="@0@"'.format(datadir),
    '-DSYSCONFDIR="@0@"'.format(sysconfdir),
    '-DLOCALEDIR="@0@"'.format(localedir)
  ] + strict_flags,
  dependencies: sdk_deps,
  include_directories: [include_directories('.'), root_inc],
  install: true,
)

libendless_gir = gnome.generate_gir(libendless,
  sources: ['endless.h'] + installed_headers + sources,
  namespace: 'Endless',
  nsversion: meson.project_version().split('.')[0],
  identifier_prefix: 'Eos',
  symbol_prefix: 'eos',
  export_packages: api_name,
  header: 'endless/endless.h',
  includes: ['GLib-2.0', 'GObject-2.0', 'Gio-2.0', 'Gtk-3.0'],
  install: true,
  extra_args: [
    '--quiet',
    '--warn-all',
    '-DCOMPILING_EOS_SDK',
  ],
)

libendless_dep = declare_dependency(sources: libendless_gir,
  link_with: libendless,
  include_directories: include_directories('.'),
  dependencies: sdk_deps,
)
