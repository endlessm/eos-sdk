#!/bin/bash

# Configure git
echo "Configuring git..."
# Change to your liking
git config --global color.ui auto
git config --global core.editor vim
git config --global alias.st status
git config --global alias.ch checkout
git config --global alias.get 'fetch --prune'
git config --global alias.publish 'push --set-upstream origin HEAD'
git config --global push.default simple
git config --global credential.helper 'cache --timeout=28800'
# Might want to change that last one to "credential.helper store" if you are
# using 2-factor authentication and authenticating with an access token.

# Uncomment and replace the lines below with your name and e-mail
# git config --global user.name 'My Name'
# git config --global user.email 'my@email'

echo "Configuring bash..."
cp /vagrant/.bash_aliases /vagrant/.bashrc /home/vagrant

echo "Installing jhbuild..."
git clone git://git.gnome.org/jhbuild
pushd jhbuild
mkdir -p m4 build-aux
./autogen.sh
make
make install
popd

echo "Configuring jhbuild..."
cp /vagrant/.jhbuildrc /vagrant/eos-sdk.modules /home/vagrant
mkdir -p $HOME/install/share/glib-2.0/schemas

echo "Installing turbine..."
git clone git://git.gnome.org/turbine
pushd turbine
python setup.py install
popd

echo "Configuring eos-shell..."
# Unbreak my app, please
gsettings set org.gnome.shell no-default-maximize true
# Turn off screen lock and blanking
gsettings set org.gnome.desktop.screensaver lock-enabled false
gsettings set org.gnome.desktop.session idle-delay 0
# Put dev tools on desktop
gsettings set org.gnome.shell icon-grid-layout "$(cat /vagrant/desktop-layout.json)"

echo "Configuring Sublime Text..."
mkdir -p $HOME/.config/sublime-text-2
pushd $HOME/.config/sublime-text-2
# Install Package Control
mkdir -p Packages Installed\ Packages
wget -O Installed\ Packages/Package\ Control.sublime-package \
    https://sublime.wbond.net/Package%20Control.sublime-package
# Install Philip's default Sublime Text configuration - feel free to create or
# fork your own settings repo and install that instead
rm -rf Packages/User
git clone https://github.com/ptomato/st2settings Packages/User
# Patch SublimeLinter's "node" executable name because Ubuntu calls it "nodejs"
python <<EOF
import json
filename = 'Packages/User/SublimeLinter.sublime-settings'
with open(filename) as f:
    obj = json.load(f)
obj[u'sublimelinter_executable_map'] = {u'javascript': u'/usr/bin/nodejs'}
with open(filename, 'w') as f:
    json.dump(obj, f, indent=4, separators=(',', ': '))
EOF
popd
