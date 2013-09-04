#!/bin/bash -ex

# Script to be run by pdebuild using the --pbuildersatisfydepends flag

# Install any dependencies that have been deposited in a specific folder
# In Jenkins, the packages can be retrieved from the output of another job
# using the copy-artifacts plugin
# Note that "dpkg -i" will fail due to missing dependencies,
# which will get picked up by "apt-get install -f"
set +e
dpkg -i deps/*.deb
set -e
apt-get install -yf

# Propagate to the default script used by pbuilder to satisfy dependencies
/usr/lib/pbuilder/pbuilder-satisfydepends
