#!/bin/bash

licenses=("publicdomain/zero/1.0/" \
    "licenses/by/2.0" \
    "licenses/by/3.0" \
    "licenses/by/4.0" \
    "licenses/by-nc/2.0" \
    "licenses/by-nc/3.0" \
    "licenses/by-nc-sa/2.0" \
    "licenses/by-nd/2.0" \
    "licenses/by-nd/3.0" \
    "licenses/by-sa/2.0" \
    "licenses/by-sa/2.5" \
    "licenses/by-sa/3.0" \
    "licenses/by-sa/4.0")

files=("legalcode" "deed.ar" "deed.en" "deed.es" "deed.fr" "deed.pt_BR")

# Remove log file if exists
rm -f wget-cc.log

# Download legalcode and deed files for each license type and version
for license in "${licenses[@]}"
do
    echo $license
    for file in "${files[@]}"
    do
        echo "  " $file
        wget --recursive --level=1 --adjust-extension --page-requisites --no-clobber --no-verbose https://creativecommons.org/$license/$file --append-output=wget-cc.log
    done
done
