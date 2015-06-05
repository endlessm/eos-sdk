#!/usr/bin/env python

import os
import shutil
import sys

def main(argv):
    source_dir = 'creativecommons.org/'

    # Organize Public domain license files
    publicdomain_licenses = [
        {
            'name': 'zero/1.0',
            'code': 'CC0-1.0'
        }
    ]
    copy_license_files(source_dir + 'publicdomain/', 'publicdomain/', publicdomain_licenses)

    # Organize Creative Commons license files
    cc_licenses = [
        {
            'name': 'by/2.0',
            'code': 'CC-BY-2.0'
        },
        {
            'name': 'by/3.0',
            'code': 'CC-BY-3.0'
        },
        {
            'name': 'by/4.0',
            'code': 'CC-BY-4.0'
        },
        {
            'name': 'by-nc/2.0',
            'code': 'CC-BY-NC-2.0'
        },
        {
            'name': 'by-nc/3.0',
            'code': 'CC-BY-NC-3.0'
        },
        {
            'name': 'by-nc-sa/2.0',
            'code': 'CC-BY-NC-SA-2.0'
        },
        {
            'name': 'by-nd/2.0',
            'code': 'CC-BY-ND-2.0'
        },
        {
            'name': 'by-nd/3.0',
            'code': 'CC-BY-ND-3.0'
        },
        {
            'name': 'by-sa/2.0',
            'code': 'CC-BY-SA-2.0'
        },
        {
            'name': 'by-sa/2.5',
            'code': 'CC-BY-SA-2.5'
        },
        {
            'name': 'by-sa/3.0',
            'code': 'CC-BY-SA-3.0'
        },
        {
            'name': 'by-sa/4.0',
            'code': 'CC-BY-SA-4.0'
        },
    ]
    copy_license_files(source_dir + 'licenses/', 'creativecommons/', cc_licenses)

    copy_requisite_files(source_dir, './')

def copy_license_files(source_dir, target_dir, licenses):

    langs = ['ar', 'en', 'es', 'fr', 'pt_BR']

    ensure_target_dirs_exist(target_dir, langs)

    for license in licenses:
        license_name = license['name']
        license_code = license['code']

        source_license_dir = source_dir + license_name + '/'

        target_legalcode_dir = target_dir + 'legalcode/'

        # Move legalcode file for license_name
        source_legalcode_path = source_license_dir + 'legalcode.html'
        target_legalcode_path = target_legalcode_dir + license_code + '-legalcode.html'
        print 'cp ' + source_legalcode_path + ' ' + target_legalcode_path
        shutil.copy(source_legalcode_path, target_legalcode_path)

        # Move deeds for individual languages
        for lang in langs:
            target_lang_dir = target_dir + lang + '/'
            source_deed_path = source_license_dir + 'deed.' + lang + '.html'
            target_deed_path = target_lang_dir + license_code + '.html'

            # Move deed file for license_name/language
            print 'cp ' + source_deed_path + ' ' + target_deed_path
            shutil.copy(source_deed_path, target_deed_path)

    # Rename 'en' subdir to default locale 'C'
    print 'mv -f ' + target_dir + 'en/ ' + target_dir + 'C/'
    shutil.rmtree(target_dir + 'C', ignore_errors=True)
    shutil.move(target_dir + 'en', target_dir + 'C')

def copy_requisite_files(source_dir, target_dir):
    for req in ['images/', 'includes/']:
        print 'cp ' + source_dir + req + ' ' + target_dir + req
        shutil.copytree(source_dir + req, target_dir + req)

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

if __name__ == '__main__':
    main(sys.argv[1:])
