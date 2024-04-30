#!/usr/bin/env bash

sudo apt update && sudo apt install curl python3 python3-pip python-is-python3

python3 -m pip install --user mercurial  # to this user

curl https://hg.mozilla.org/mozilla-central/raw-file/default/python/mozboot/bin/bootstrap.py -O
python3 bootstrap.py

cd mozilla-unified
# I had problems with building the older version
# hg checkout f4922b9e9a6b # don't checkout the challenge works fine in the latest version

# hg import ../patch --no-commit  # I applied patch manually like a caveman

MOZCONFIG=./mozconfig ./mach build