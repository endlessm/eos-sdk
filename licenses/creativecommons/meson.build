languages = [
  'C',
  'ar',
  'es',
  'fr',
  'pt_BR',
]

licenses = [
  'CC-BY-2.0',
  'CC-BY-3.0',
  'CC-BY-4.0',
  'CC-BY-NC-2.0',
  'CC-BY-NC-3.0',
  'CC-BY-NC-SA-2.0',
  'CC-BY-ND-2.0',
  'CC-BY-ND-3.0',
  'CC-BY-SA-2.0',
  'CC-BY-SA-2.5',
  'CC-BY-SA-3.0',
  'CC-BY-SA-4.0',
]

foreach lang: languages
  license_files = []
  foreach license: licenses
    license_files += '@0@/@1@.html'.format(lang, license)
  endforeach
  install_data(license_files,
    install_dir: join_paths(datadir, 'licenses', 'creativecommons', lang),
  )
endforeach

# Legalcode
legalcode_files = []
foreach license: licenses
  legalcode_files += 'legalcode/@0@-legalcode.html'.format(license)
endforeach
install_data(legalcode_files,
  install_dir: join_paths(datadir, 'licenses', 'creativecommons', 'legalcode'),
)
