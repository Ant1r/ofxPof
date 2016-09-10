#!/bin/bash

rm -rf pof/
mkdir pof/
echo copying help patches...
cp -a ../help/* pof/
#if [ -n "`uname -a | grep x86_64`" ]
#	then cp bin/pof.pd_linux pof/pof.l_ia64
#	else cp bin/pof.pd_linux pof/pof.l_i386
#fi
echo copying pof external...
cp bin/pof.* pof/

mkdir pof/libs
echo copying libs...
cp -a bin/libs/* pof/libs

LIBS_TO_COPY="freeimage boost_filesystem boost_system openal gstreamer gstbase gstapp gstvideo sndfile"

for libtocopy in $LIBS_TO_COPY ; do 
	libfile=`ldd bin/pof.* | grep lib${libtocopy} | cut -d' ' -f 3`
	echo "$libtocopy : copying $libfile in libs"
	cp $libfile pof/libs
	done

	
POF_VERSION_MAJOR=`grep POF_VERSION_MAJOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_MINOR=`grep POF_VERSION_MINOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_PATCH=`grep POF_VERSION_PATCH ../src/version.h | cut -d' ' -f 3`

if [ x$1 == xtest ] ; then
	deken package --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
else
	deken upload --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
	rm -rf pof/
fi


