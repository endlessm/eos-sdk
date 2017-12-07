languages = [
  'C',
  'ar',
  'es',
  'fr',
  'pt_BR',
]

licenses = [
  'CC0-1.0',
]

foreach lang: languages
  license_files = []
  foreach license: licenses
    license_files += '@0@/@1@.html'.format(lang, license)
  endforeach
  install_data(license_files,
    install_dir: join_paths(datadir, 'licenses', 'publicdomain', lang),
  )
endforeach

# Legalcode
legalcode_files = []
foreach license: licenses
  legalcode_files += 'legalcode/@0@-legalcode.html'.format(license)
endforeach
install_data(legalcode_files,
  install_dir: join_paths(datadir, 'licenses', 'publicdomain', 'legalcode'),
)
