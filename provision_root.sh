#!/bin/bash

# Public repositories
add-apt-repository -y "deb http://archive.canonical.com/ubuntu $(lsb_release -sc) partner"
add-apt-repository -y ppa:gnome3-team/gnome3
add-apt-repository -y ppa:webupd8team/sublime-text-2
apt-get update
apt-get -y dist-upgrade
# Various dependencies
apt-get -y install git devhelp sublime-text python-pip emacs fluxbox zsh tmux \
    sl osc pyflakes pep8 gnome-devel-docs gtk-doc-tools libgirepository1.0-dev \
    gnome-api-docs python-bs4 gnome-common yelp-tools
# For testing this script: dependencies of the rest of the provisioning scripts
# apt-get -y install git gnome-common yelp-tools sublime-text devhelp python-pip
apt-get clean

Create shortcuts that will show up in the EOS app store?
echo "Creating shortcuts for dev tools in EOS app store..."
for app in gnome-terminal devhelp sublime-text-2; do
    cp /usr/share/applications/$app.desktop /usr/share/applications/eos-app-$app.desktop
done

echo "Installing Transifex client..."
pip install transifex-client
