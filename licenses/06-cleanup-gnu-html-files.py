#!/usr/bin/env python3

# Copyright (C) 2016 Endless Mobile, Inc.

from bs4 import BeautifulSoup
import os
import re
import sys

from license_utils import rewrite_attr

def main(argv):
    # Clean up GNU license files
    gnu_licenses = [
        'FDL-1.1.html',
        'FDL-1.2.html',
        'FDL-1.3.html',
    ]

    for license in gnu_licenses:
        cleanup_license_file('gnu/C/', license)

    cleanup_css_file('gnu/css/layout.min.css')

def cleanup_license_file(src_dir, license):
    f_license = os.path.join(src_dir, license)
    print('Cleaning up ' + f_license)

    with open(f_license, 'r+') as f:
        html = f.read()
        soup = BeautifulSoup(html, from_encoding='UTF-8')

        # Remove unwanted alternate links
        for unwanted_link in soup.findAll('link', {'rel': 'alternate'}):
            unwanted_link.extract()

        # Remove unwanted divs from header
        for unwanted_div in ['toplinks', 'searcher', 'translations', 'fsf-frame', 'navigation', 'fsf-links', 'footer']:
            for toplinks in soup.findAll('div', {'id': unwanted_div}):
                toplinks.extract()

        # Remove list of links to related pages.
        for li in soup.select('ul li a'):
            li.parent.parent.extract()
        for h2 in soup(text=re.compile(r'Table of Contents')):
            h2.extract()
        for h2 in soup(text=re.compile(r'Related Pages')):
            h2.parent.extract()
        for notes in soup(text=re.compile(r'diff files showing the changes between')):
            notes.parent.extract()

        # Remove unwanted hr
        for unwanted_hr in soup.findAll('hr', {'style': None}):
            unwanted_hr.extract()

        # Make attributes relative
        rewrite_attr(soup, 'link', 'href', '^/graphics/', '../images/')
        rewrite_attr(soup, 'img', 'src', '^/graphics/', '../images/')
        rewrite_attr(soup, 'link', 'href', '^/', '../css/')

        # Overwrite license file with clean version of html
        html = soup.prettify(soup.original_encoding)
        f.seek(0)
        f.truncate()
        f.write(html)

def cleanup_css_file(f_css):
    print('Cleaning up ' + f_css)

    with open(f_css, 'r+') as f:
        css = f.read()
        css = re.sub(r'/graphics/', '../images/', css)
        f.seek(0)
        f.truncate()
        f.write(css)

if __name__ == '__main__':
    main(sys.argv[1:])
