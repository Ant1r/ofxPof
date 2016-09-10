#!/bin/bash

export PATH+=:~/bin/

rm -rf pof/
mkdir pof/

echo copying help patches...
cp -a ../help/* pof/

echo copying example patches...
mkdir pof/example/
cp -a ../example/pd/* pof/example/

#if [ -n "`uname -a | grep x86_64`" ]
#	then cp bin/pof.pd_linux pof/pof.l_ia64
#	else cp bin/pof.pd_linux pof/pof.l_i386
#fi
cp ~/Library/Pd/pof/pof.pd_darwin pof/
cp ../../../export/osx/libs/libfmodex.dylib pof/
	
POF_VERSION_MAJOR=`grep POF_VERSION_MAJOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_MINOR=`grep POF_VERSION_MINOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_PATCH=`grep POF_VERSION_PATCH ../src/version.h | cut -d' ' -f 3`

if [ x$1 == xtest ] ; then
	deken package --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
else
	deken upload --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
	rm -rf pof/
fi


