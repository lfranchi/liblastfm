#!/bin/sh
cd `dirname $0`/..
for x in `admin/findsrc.rb cpp h`
do
    tab2space -unix $x $x
    #(echo ',s/ *$//'; echo 'wq') | ed -s $x
done