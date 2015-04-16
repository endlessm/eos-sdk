#!/usr/bin/env python

import os
import shutil
import sys

def main(argv):
    source_dir = 'creativecommons.org/'
    target_dir = 'creativecommons/'

    copy_license_files(source_dir + 'licenses/', target_dir)
    copy_requisite_files(source_dir, target_dir)

def copy_license_files(source_dir, target_dir):

    licenses = [
        'by/3.0',
        'by/4.0',
        'by-sa/3.0',
        'by-sa/4.0',
        'by-nd/2.0',
        'by-nd/3.0',
    ]

    langs = ['ar', 'en', 'es', 'fr', 'pt_BR']

    ensure_target_dirs_exist(target_dir, langs)

    for license in licenses:
        license_code = get_code_for_license(license)

        source_license_dir = source_dir + license + '/'

        target_legalcode_dir = target_dir + 'legalcode/'

        # Move legalcode file for license
        source_legalcode_path = source_license_dir + 'legalcode.html'
        target_legalcode_path = target_legalcode_dir + license_code + '-legalcode.html'
        print 'cp ' + source_legalcode_path + ' ' + target_legalcode_path
        shutil.copy(source_legalcode_path, target_legalcode_path)

        # Move deeds for individual languages
        for lang in langs:
            target_lang_dir = target_dir + lang + '/'
            source_deed_path = source_license_dir + 'deed.' + lang + '.html'
            target_deed_path = target_lang_dir + license_code + '.html'

            # Move deed file for license/language
            print 'cp ' + source_deed_path + ' ' + target_deed_path
            shutil.copy(source_deed_path, target_deed_path)

    # Rename 'en' subdir to default locale 'C'
    print 'mv -f ' + target_dir + 'en/ ' + target_dir + 'C/'
    shutil.rmtree(target_dir + 'C', ignore_errors=True)
    shutil.move(target_dir + 'en', target_dir + 'C')

def copy_requisite_files(source_dir, target_dir):
    for req in ['images/', 'includes/']:
        print 'cp ' + source_dir + req + ' ' + target_dir + req
        try:
            shutil.rmtree(source_dir + req, ignore_errors=True)
        except IOError:
            pass
        shutil.copytree(target_dir + req, source_dir + req)

def ensure_target_dirs_exist(target_dir, langs):
    # Ensure target directory exists
    try:
        os.makedirs(target_dir)
    except OSError:
        pass

    # Create target language directories
    for lang in langs:
        lang_path = os.path.join(target_dir, lang)
        try:
            os.makedirs(lang_path)
        except OSError:
            pass

    # Create target legalcode directory
    legalcode_path = os.path.join(target_dir, 'legalcode')
    try:
        os.makedirs(legalcode_path)
    except OSError:
        pass

def get_code_for_license(license):
    return 'CC-' + license.upper().replace('/', '-')

if __name__ == '__main__':
    main(sys.argv[1:])
