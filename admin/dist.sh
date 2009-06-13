#!/bin/bash

v=`perl -ne '/VERSION\s*=\s*((\d\.)*\d)/ and print \$1' $0/../../src/lastfm.pro`
basename=liblastfm-$v

# OSX copies special file attributes to ._files when you tar!
export COPY_EXTENDED_ATTRIBUTES_DISABLE=true
export COPYFILE_DISABLE=true

git archive --prefix=$basename/ HEAD | bzip2 > $basename.tar.bz2

echo `pwd`/$basename.tar.bz2