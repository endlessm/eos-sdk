#!/bin/bash -e

PORT=2200
HOST="ci@ci.endlessm.com"
BRANCH=master
DEST_DIR=$HOME
ARTIFACTS="app-store celebrities cooking english file-manager fitness futbol \
    guatemala khan-academy media mexico photos plymouth-theme programming-app \
    pstack resume-app science sdk shell shell-apps shell-apps-x86-extra \
    shell-browser shell-explore social theme translation travel typing weather \
    wikipedia wikipedia-brazil wikipedia-health wikipedia-offline youtube"
DATABASES="brazil celebrities cooking fitness general futbol health science \
    travel"

for artifact in $ARTIFACTS; do
    ARTIFACT_LINKS="$ARTIFACT_LINKS /var/lib/jenkins/jobs/eos-$artifact-$BRANCH/lastSuccessful/archive/build/*.deb"
done
for database in $DATABASES; do
    ARTIFACT_LINKS="$ARTIFACT_LINKS /var/lib/jenkins/jobs/eos-wikipedia-db-$database-$BRANCH/lastSuccessful/archive/build/*.tar.gz"
done

set +e
    echo "Removing any old packages..."
    rm -rf $DEST_DIR/*.deb $DEST_DIR/*.tar.gz

    echo "Downloading artifacts from Jenkins..."
    scp -P $PORT -r -oStrictHostKeyChecking=no $HOST:"$ARTIFACT_LINKS" $DEST_DIR

    echo "Installing EOS packages..."
    # Remove out debugging packages
    rm -rf $DEST_DIR/*-dbg*.deb
    # Install EOS packages
    sudo dpkg -i $DEST_DIR/*.deb
set -e

# Fix conflicts and install dependencies of EOS packages
sudo apt-get install -yf

# Install the database files
pushd /var
set +e
    for db_archive in $DEST_DIR/*.tar.gz; do
        echo "Extracting $db_archive...";
        sudo tar xzf $db_archive;
    done
set -e
popd

echo "Configuring elasticsearch..."
sudo chown -R elasticsearch. /var/lib/elasticsearch
sudo /usr/share/elasticsearch/bin/plugin -install elasticsearch/elasticsearch-analysis-phonetic/1.6.0
sudo /usr/share/elasticsearch/bin/plugin -install mobz/elasticsearch-head

echo "Configuring personality..."
sudo eos-select-personality default

echo "Configuring chromium..."
if [ -d ~/.config/chromium ]; then
    rm -rf ~/.config/chromium.old
    mv ~/.config/chromium ~/.config/chromium.old
fi
cp -r /etc/skel/.config/chromium ~/.config

echo "Quiet time before reboot..."
sleep 10

echo "Rebooting"
sudo reboot

echo "Done"
