#!/bin/bash

# Public repositories
add-apt-repository -y "deb http://archive.canonical.com/ubuntu $(lsb_release -sc) partner"
add-apt-repository -y ppa:gnome3-team/gnome3
add-apt-repository -y ppa:webupd8team/sublime-text-2
apt-get update
apt-get -y dist-upgrade
# Various dependencies
apt-get -y install git devhelp sublime-text python-pip yelp-tools fluxbox tmux \
    sl osc pyflakes pep8 gnome-devel-docs gtk-doc-tools libgirepository1.0-dev \
    gnome-api-docs librsvg2-dev gnome-common language-pack-pt language-pack-es \
    zsh emacs python-bs4 libgtk-3-dev libgtk2.0-dev libclutter-1.0-dev \
    libjson-glib-doc python-polib
# For testing this script: dependencies of the rest of the provisioning scripts
# apt-get -y install git gnome-common yelp-tools sublime-text devhelp python-pip
apt-get clean

# Repositories that require VPN if you are not in the San Francisco office are
# below. If you are in the office, you can comment out the following stanza.
# Note that the VPN has to be connected on your host machine too.
resolvconf -a eth0 <<EOF
nameserver 10.0.1.9
search endlessm-sf.com
EOF

add-apt-repository -ys "deb http://obs-master:82/shared/eos dev extra endless"
apt-get update
apt-get -y --allow-unauthenticated dist-upgrade
# Requires interactivity when updating base-passwd?
apt-get -y --allow-unauthenticated install eos-guatemala endlessos-base-photos \
    eos-celebrities eos-cooking eos-english eos-exploration-center eos-fitness \
    eos-file-manager eos-translation eos-shell eos-khanacademy eos-programming \
    eos-mexico eos-resume eos-typing eos-wikipedia-health eos-wikipedia-brazil \
    eos-futbol eos-travel eos-science eos-weather chromium-browser eos-youtube \
    eos-social eos-wikipedia eos-language-pack-es eos-language-pack-pt
# Requires selecting GDM instead of LightDM
# Don't install eos-shell-apps so that we don't pull in all the giant games deps
# like OpenArena, and don't install eos-media because it's huge
apt-get clean

# Create shortcuts that will show up on the EOS desktop
echo "Creating shortcuts for dev tools for EOS desktop..."
for app in gnome-terminal devhelp sublime-text-2; do
    cp /usr/share/applications/$app.desktop \
        /usr/share/applications/eos-app-$app.desktop
done

echo "Installing Transifex client..."
pip install transifex-client
