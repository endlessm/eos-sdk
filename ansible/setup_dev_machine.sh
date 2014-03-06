#!/bin/bash -e

DEFAULT_USER="endless"
DEFAULT_PASSWORD="endless"
DEFAULT_IP="192.168.56.101"
DEFAULT_PORT="22"

echo "To run this script, you need to have the following"
echo " - VM running with the correct dev HD image"
echo " - A user defined with sudo privileges"
echo " - sshpass installed on the local machine"
echo
read -p "Press [Enter] to continue..."

if [[ ! $(which sshpass) ]]; then
  echo "sshpass not found. Exiting"
  echo "You can install it with sudo apt-get install sshpass"
  exit 1
fi

echo -n "[Default: $DEFAULT_IP] "
read -e -p "Enter your machine's IP: " TARGET_IP
if [[ -z "$TARGET_IP" ]]; then
    TARGET_IP="$DEFAULT_IP"
fi
echo -n "[Default: $DEFAULT_PORT] "
read -e -p "Enter your machine's SSH port: " TARGET_PORT
if [[ -z "$TARGET_PORT" ]]; then
    TARGET_PORT="$DEFAULT_PORT"
fi
echo -n "[Default: $DEFAULT_USER] "
read -e -p "Enter your machine's user: " TARGET_USER
if [[ -z "$TARGET_USER" ]]; then
    TARGET_USER="$DEFAULT_USER"
fi
echo -n "[Default: $DEFAULT_PASSWORD] "
read -e -p "Enter your machine's password: " TARGET_PASS
if [[ -z "$TARGET_PASS" ]]; then
    TARGET_PASS="$DEFAULT_PASSWORD"
fi

API_KEY=""
while [[ -z $API_KEY ]]; do
  read -e -p "Enter your Github Personal Access Token (github.com/settings/applications): " API_KEY
done

echo

echo -n "Checking connectivity to VM/machine..."
sshpass -p "${TARGET_PASS}" ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -p $TARGET_PORT $TARGET_USER@$TARGET_IP 'uname -a' &> /dev/null
echo "[OK]"

echo -n "Checking API key..."
 git ls-remote -h https://$API_KEY@github.com/endlessm/eos-sdk &> /dev/null
echo "[OK]"
echo


tmp_inventory=$(mktemp setupdevXXXXXX)
trap "rm -f $tmp_inventory" EXIT

echo "Creating Ansible inventory file in $tmp_inventory"
cat << EOF > $tmp_inventory
[dev_machine]
dev_machine ansible_ssh_host=$TARGET_IP ansible_ssh_port=$TARGET_PORT ansible_ssh_user="$TARGET_USER" ansible_sudo_pass="${TARGET_PASS}" ansible_ssh_pass="${TARGET_PASS}" ansible_connection=ssh
EOF

ansible-playbook -i $tmp_inventory playbooks/setup_dev_machine_root.yaml
ansible-playbook -i $tmp_inventory playbooks/setup_dev_folder.yaml
ansible-playbook -i $tmp_inventory playbooks/kill_shell.yaml
ansible-playbook -i $tmp_inventory playbooks/setup_jhbuild.yaml
ansible-playbook -i $tmp_inventory playbooks/install_jhbuild_deps.yaml
ansible-playbook -i $tmp_inventory playbooks/jhbuild_run.yaml --extra-vars "api_key=$API_KEY"

echo "Repos are cloned. You should be able to run 'jhbuild build' on the target"
