#!/bin/bash

export PATH+=:~/bin/

rm -rf pof/
mkdir pof/

echo ; echo copying help patches...
cp -a ../help/* pof/
cp ../LICENSE.txt pof/

echo ; echo copying example patches...
mkdir pof/example/
cp -a ../example/pd/* pof/example/

echo ; echo copying pof external...
cp ~/Library/Pd/pof/pof.pd_darwin pof/
echo ; echo copying libs...
cp ../../../export/osx/libs/libfmodex.dylib pof/

POF_VERSION=`strings pof/pof.pd_darwin | grep "Pof: version" | cut -s -d' ' -f 4`

if [ x$1 == xtest ] ; then
	deken package --version ${POF_VERSION} pof
else
	deken upload --no-source-error --version ${POF_VERSION} pof
	rm -rf pof/
fi


