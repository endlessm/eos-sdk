#!/bin/bash

licenses=("old-licenses/fdl-1.1.html" \
	"old-licenses/fdl-1.2.html" \
	"fdl-1.3.html")

# Remove log file if exists
rm -f wget-gnu.log

# Download license file for each license type
for license in "${licenses[@]}"
do
    echo $license
	wget --adjust-extension --page-requisites --no-clobber --no-verbose http://www.gnu.org/licenses/$license --append-output=wget-gnu.log
done
