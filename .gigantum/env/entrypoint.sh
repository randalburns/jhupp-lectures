#!/bin/bash

USER_ID=${LOCAL_USER_ID:-9001}

echo "Starting with UID: $USER_ID"
useradd --shell /bin/bash -u $USER_ID -o -c "" -m giguser
export HOME=/home/giguser

# Setup /mnt/ as a safe place to put user runnable code
mkdir /mnt/labbook
chown -R giguser:root /mnt/labbook

# Setup docker sock to run as the user
chown giguser:root /run/docker.sock
chmod 777 /var/run/docker.sock

export JUPYTER_RUNTIME_DIR=/mnt/share/jupyter/runtime
chown -R giguser:root /mnt/share/

# Run the Docker Command
exec gosu giguser "$@"
