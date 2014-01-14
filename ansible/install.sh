#!/bin/bash -e

ANSIBLE_DIR="/etc/ansible"
DEPENDENCIES="make python-pip python-dev git sshpass"

if [[ $EUID -ne 0 ]]; then
  echo "This script must be run as sudo or root. Exiting."
  exit 1
fi

read -p "Are you sure you want to install the server config? " -n 1 -r
echo   # Move to a new line
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
  echo "Install canceled. Exiting."
  exit 1
fi

echo "Checking dependencies"
set +e
  dpkg -s $DEPENDENCIES &> /dev/null
  has_dependencies=$?
set -e

if [[ $has_dependencies -ne 0 || ! $(which ansible) ]]; then
  echo "Installing $DEPENDENCIES"
  apt-get update
  apt-get install -y --force-yes $DEPENDENCIES
  echo "Dependencies installed"

  echo "Installing ansible"
  pip install PyYAML jinja2 paramiko
  rm -rf ${HOME}/apps/ansible
  sudo -u $SUDO_USER -- mkdir -p ${HOME}/apps
  pushd ${HOME}/apps
    sudo -u $SUDO_USER -- git clone https://github.com/ansible/ansible.git
    cd ansible
    make install
  popd
  mkdir -p /etc/ansible
fi

if [[ ! $(which ansible-playbook) ]]; then
  echo "Installing playbook script softlink"
  ln -fs ${HOME}/apps/ansible/build/scripts-2.7/ansible-playbook /usr/bin/
fi

echo "Checking if this user has a ssh key"
ssh_key="$HOME/.ssh/id_rsa"
ssh_public_key="$ssh_key.pub"
ssh_keygen_cmd="ssh-keygen -b 4096 -f $ssh_key"

if [[ ! -f $ssh_public_key ]]; then
  echo "No ssh key found. Regenerating"
  echo "Using $ssh_keygen_cmd to create key"
  sudo -u $SUDO_USER -- $ssh_keygen_cmd
fi

echo "Removing old files and links"
rm -rf $ANSIBLE_DIR/hosts

echo "Soft-linking relevant files"
cwd=$(dirname $0)
abs_cwd=$(readlink -f $cwd)

echo "Current directory: $abs_cwd"
ln -s $abs_cwd/hosts $ANSIBLE_DIR/hosts

echo "Checking connectivity. If any failures appear below, it means that \
 you need to push your ssh keys to those hosts with ssh-copy-id <user>@<machine>"

sudo -u $SUDO_USER ansible all -m ping
