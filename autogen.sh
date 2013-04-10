# Bootstrap script for Open Endless SDK
# Run this script on a clean source checkout to get ready for building.

mkdir -p m4
gtkdocize || exit 1
autoreconf -fi

