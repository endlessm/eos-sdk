#!/bin/bash

# For add-apt-repository
apt-get -y install software-properties-common

add-apt-repository -y "deb http://archive.canonical.com/ubuntu $(lsb_release -sc) partner"
add-apt-repository -y ppa:gnome3-team/gnome3
add-apt-repository -y ppa:webupd8team/sublime-text-2
apt-get update
apt-get -y dist-upgrade

# Various dependencies
apt-get -y install git vim emacs sublime-text gnome-terminal gnome-common \
    devhelp gtk-doc-tools libgirepository1.0-dev librsvg2-dev sl yelp-tools \
    gnome-api-docs gnome-devel-docs python-bs4 zsh fluxbox tmux \
    language-pack-pt language-pack-gnome-pt language-pack-es \
    language-pack-gnome-es pep8 pyflakes vpnc chromium-browser

# # For testing: dependencies of the rest of the provisioning scripts
# apt-get -y install git chromium-browser devhelp sublime-text gnome-terminal

# Leave commented out until there is a newer version than 4.2.12 available via
# the Ubuntu package repository
# apt-get -y install virtualbox-guest-dkms virtualbox-guest-x11 \
#    virtualbox-guest-utils

Create shortcuts that will show up in the EOS app store?
echo "Creating shortcuts for dev tools in EOS app store..."
for app in gnome-terminal devhelp sublime-text-2; do
    cp /usr/share/applications/$app.desktop /usr/share/applications/eos-app-$app.desktop
done
