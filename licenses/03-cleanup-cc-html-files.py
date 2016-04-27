#!/usr/bin/env python3

# Copyright (C) 2016 Endless Mobile, Inc.

from bs4 import BeautifulSoup, Comment
import os
import re
import sys

from license_utils import rewrite_attr

def main(argv):
    langs = ['C', 'ar', 'es', 'fr', 'pt_BR']

    # Clean up Public domain license files
    cleanup_legalcode_file('publicdomain/', 'CC0-1.0')
    for lang in langs:
        cleanup_deed_file('publicdomain/', 'CC0-1.0', lang)
    print('')

    # Clean up Creative Commons license files
    cc_licenses = [
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

    for license in cc_licenses:
        cleanup_legalcode_file('creativecommons/', license)
        for lang in langs:
            cleanup_deed_file('creativecommons/', license, lang)
        print('')

def cleanup_legalcode_file(src_dir, license):
    f_legalcode = os.path.join(src_dir, 'legalcode', license + '-legalcode.html')
    print('Cleaning up ' + f_legalcode)

    with open(f_legalcode, 'r+') as f:
        html = f.read()
        soup = BeautifulSoup(html, from_encoding="UTF-8")

        # Remove IE7 conditional comments
        cleanup_conditional_comments(soup)

        # Remove errata.js script
        for script in soup.findAll('script'):
            if 'errata' in script['src']:
                script.extract()

        # Make attributes relative
        rewrite_attr(soup, 'img', 'src', '.*creativecommons.org/images/', '../../images/')
        rewrite_attr(soup, 'img', 'src', '^/images/', '../../images/')
        rewrite_attr(soup, 'link', 'href', '.*creativecommons.org/includes/', '../../includes/')
        rewrite_attr(soup, 'link', 'href', '^/includes/', '../../includes/')
        rewrite_attr(soup, 'script', 'src', '.*creativecommons.org/includes/', '../../includes/')
        rewrite_attr(soup, 'a', 'href', '^creativecommons.org/', 'http://creativecommons.org/')
        rewrite_attr(soup, 'a', 'href', '^//creativecommons.org/', 'http://creativecommons.org/')

        # Remove footer
        for foot in soup.findAll('div', {'id': 'deed-foot'}):
            foot.extract()

        # Overwrite legalcode file with clean version of html
        html = soup.prettify(soup.original_encoding)
        f.seek(0)
        f.truncate()
        f.write(html)
        f.close()

def cleanup_deed_file(src_dir, license, lang):
    f_deed = os.path.join(src_dir, lang, license + '.html')
    print('Cleaning up ' + f_deed)

    with open(f_deed, 'r+') as f:
        html = f.read()
        soup = BeautifulSoup(html, from_encoding="UTF-8")

        # Remove IE7 conditional comments
        cleanup_conditional_comments(soup)

        # Remove RDF declarations
        cleanup_rdf_declarations(soup)

        # Remove deed.js script
        for script in soup.findAll('script', src=True):
            if 'scraper.creativecommons.org/js/deed.js' in script['src']:
                script.extract()

        # Make attributes relative
        rewrite_attr(soup, 'a', 'href', '.*legalcode$', '../legalcode/' + license + '-legalcode.html')
        rewrite_attr(soup, 'a', 'href', '^/choose/', 'http://creativecommons.org/choose/')
        rewrite_attr(soup, 'img', 'src', '.*creativecommons.org/images/', '../../images/')
        rewrite_attr(soup, 'img', 'src', '^/images/', '../../images/')
        rewrite_attr(soup, 'link', 'href', '.*creativecommons.org/includes/', '../../includes/')
        rewrite_attr(soup, 'link', 'href', '^/includes/', '../../includes/')
        rewrite_attr(soup, 'script', 'src', '.*creativecommons.org/includes/', '../../includes/')
        rewrite_attr(soup, 'script', 'src', '^/includes/', '../../includes/')
        rewrite_attr(soup, 'script', 'src', '^//scraper.creativecommons.org/js/deed.js', '../../includes/deed.js')

        # Remove inline JS
        for script in soup.findAll('script'):
            if not script.has_key('src'):
                script.extract()

        # Remove languages footer
        for lang_footer in soup.findAll('div', {'id': 'languages'}):
            lang_footer.extract()

        # Overwrite deed file with clean version of html
        html = soup.prettify(soup.original_encoding)
        f.seek(0)
        f.truncate()
        f.write(html)
        f.close()

def cleanup_conditional_comments(html):
    comments = html.findAll(text=lambda text:isinstance(text, Comment) and '[if' in text)
    for comment in comments:
        comment.extract()

def cleanup_rdf_declarations(html):
    decls = html.findAll(text=lambda text:isinstance(text, Comment) and 'RDF' in text)
    for decl in decls:
        decl.extract()

    for rdf in html.findAll('link', {'href': 'rdf'}):
        rdf.extract()

if __name__ == '__main__':
    main(sys.argv[1:])
