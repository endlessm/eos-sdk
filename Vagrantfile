# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # All Vagrant configuration is done here. The most common configuration
  # options are documented and commented below. For a complete reference,
  # please see the online documentation at vagrantup.com.

  # Every Vagrant virtual environment requires a box to build off of.
  config.vm.box = "raring32-desktop"

  # The url from where the 'config.vm.box' box will be fetched if it
  # doesn't already exist on the user's system.
  config.vm.box_url = "https://dl.dropboxusercontent.com/u/4790586/raring32-desktop.box"

  # Create a forwarded port mapping which allows access to a specific port
  # within the machine from a port on the host machine. So accessing
  # "localhost:3000" on the host machine will talk to the NodeJS search API,
  # and "localhost:9200" will talk to Elasticsearch.
  config.vm.network :forwarded_port, :guest => 3000, :host => 3000
  config.vm.network :forwarded_port, :guest => 9200, :host => 9200

  # If true, then any SSH connections made will enable agent forwarding.
  # Default value: false
  # config.ssh.forward_agent = true
  config.ssh.forward_x11 = true

  # Share an additional folder to the guest VM. The first argument is
  # the path on the host to the actual folder. The second argument is
  # the path on the guest to mount the folder. And the optional third
  # argument is a set of non-required options.
  # config.vm.synced_folder "../..", "/home/vagrant"

  # Provider-specific configuration so you can fine-tune various
  # backing providers for Vagrant. These expose provider-specific options.
  # Example for VirtualBox:

  config.vm.provider :virtualbox do |vb|
    # Don't boot with headless mode
    vb.gui = true
    # Use VBoxManage to customize the VM.
    # vb.customize ["modifyvm", :id, "--memory", "2048"]
  end

  # View the documentation for the provider you're using for more
  # information on available options.

  config.vm.provision :shell, :path => "provision_root.sh"
  config.vm.provision :shell, :path => "provision_home.sh", :privileged => false

end
