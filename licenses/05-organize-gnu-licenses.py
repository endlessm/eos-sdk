#!/usr/bin/env python3

# Copyright 2016 Endless Mobile, Inc.

import os
import shutil
import sys

def main(argv):
    source_dir = 'www.gnu.org/'
    target_dir = 'gnu/'

    # Organize GNU license files
    gnu_licenses = [
        {
            'name': 'licenses/old-licenses/fdl-1.1.html',
            'code': 'FDL-1.1'
        },
        {
            'name': 'licenses/old-licenses/fdl-1.2.html',
            'code': 'FDL-1.2'
        },
        {
            'name': 'licenses/fdl-1.3.html',
            'code': 'FDL-1.3'
        }
    ]
    copy_license_files(source_dir, target_dir, gnu_licenses)

    copy_requisite_files(source_dir, target_dir)

def copy_license_files(source_dir, target_dir, licenses):
    # GNU licenses are not translated, yet we need the licenses in the C locale directory
    # for these licenses to be served by the API.
    locale_dir = os.path.join(target_dir, 'C')
    try:
        os.makedirs(locale_dir)
    except OSError:
        pass

    for license in licenses:
        license_name = license['name']
        license_code = license['code']
        source_license_path = source_dir + license_name
        target_license_path = locale_dir + '/' + license_code + '.html'

        print('cp ' + license_name + ' ' + target_license_path)
        shutil.copy(source_license_path, target_license_path)

def copy_requisite_files(source_dir, target_dir):
    shutil.copytree(source_dir + 'graphics/', target_dir + 'images/')

    try:
        os.makedirs(target_dir + 'css/')
    except OSError:
        pass

    for css in ['combo.css', 'layout.min.css', 'mini.css', 'print.min.css']:
        print('cp ' + source_dir + css + ' ' + target_dir + 'css/' + css)
        shutil.copy(source_dir + css, target_dir + 'css/' + css)

if __name__ == '__main__':
    main(sys.argv[1:])
