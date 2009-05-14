#!/bin/sh

d=`dirname $0`

for x in `$d/findsrc cpp``$d/findsrc h`
do
    tab2space -unix $x $x
done