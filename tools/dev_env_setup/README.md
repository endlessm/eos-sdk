# Using Ansible

## Principles

- Ansible is an automatic configuration management (CM) tool that helps with deploying and
configuring devices with little interaction and repeatability. Anything that one might manually
"configure" on a target device should be done through Ansible to make sure that we can with
minimal effort recreate critical parts of our infrastructure. While there are limitation to
what can be done with this tool, the benefits (currently) far outweigh the cons.

## Installation

- To fully utilize these scripts, you need to manually install Ansible 1.2+ using `install.sh`
since Ubuntu repositories only carry version 1.1. The install scripts downloads the proper
dependencies, downloads Ansible, makes it, and installs it. The script also soft-links the
configuration files from /etc to the ones in the repository. In general if you're using sudo,
you don't need to do anything special to get the script to work. Ansible is fully configured
on ansible@domain-services and the ansible test machine is on sg@10.0.1.8.

## Running

- Ansible uses configuration files (`ansible.cfg`), hosts definiton files (`hosts`), variable
definitions (`default_variables.yaml`) and playbooks (any other `yaml`s) to run scripts.
- To run a "playbook" yaml file, you need to type `ansible-playbook <playbook name>.yaml`. For
standardization, top-level scripts are in this directory while helper modules have been included
in the playbooks files.
- root@obs-repository and ansible@domain-services pulic keys have been also placed in keys/ for
reference
- In general, the account that you are using to run the ansible script needs to have its ssh key
in the target's ssh `authorized_hosts` file. You can do this by running `ssh-copy-id user@machine`
- Logs are published on the server that the script is running on. This means that migrations are
logged on ostree.endlessm.com and publishes logged on obs-repository.

## Writing/extending the scripts

- This is too broad of a topic so please refer to the current scripts and the following sites:
 - https://gist.github.com/marktheunissen/2979474
 - http://www.ansibleworks.com/docs/modules.html

## Current scripts
- setup_dev_env.sh - Installs the dev environment on the VM
