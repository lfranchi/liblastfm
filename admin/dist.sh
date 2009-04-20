#!/bin/bash

# OSX copies special file attributes to ._files when you tar!
export COPY_EXTENDED_ATTRIBUTES_DISABLE=true
export COPYFILE_DISABLE=true

dst=/tmp/liblastfm-$1

git clean -xfd
cp -R . $dst || exit 1
rm -rf $dst/.git $dst/admin/dist.sh $dst/.gitignore
cd /tmp
tar cjf liblastfm-$1.tar.bz2 liblastfm-$1
cd -
mv $dst.tar.bz2 .
rm -r $dst
