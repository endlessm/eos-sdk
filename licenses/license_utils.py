#!/usr/bin/env python

# Copyright (C) 2016 Endless Mobile, Inc.

import re

def rewrite_attr(html, elem, attr, source, target):
    for element in html.findAll(elem):
        if element.has_key(attr):
            attr_val = re.sub(source, target, element[attr])
            element[attr] = attr_val

