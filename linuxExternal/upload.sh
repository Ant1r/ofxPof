#!/bin/bash

rm -rf pof/
mkdir pof/

echo copying help patches...
cp -a ../help/* pof/
cp ../LICENSE.txt pof/

echo copying example patches...
mkdir pof/example/
cp -a ../example/pd/* pof/example/


echo copying pof external...
cp bin/pof.* pof/

patchelf --set-rpath '$ORIGIN/libs' pof/pof.*

mkdir pof/libs
echo copying libs...
cp -a bin/libs/* pof/libs

LIBS_TO_COPY="freeimage boost_filesystem boost_system openal gstreamer gstbase gstapp gstvideo sndfile GLEW openjpeg IlmImf  IlmThread Half Iex raw"

for libtocopy in $LIBS_TO_COPY ; do 
	libfile=`ldd bin/pof.* | grep lib${libtocopy} | cut -d' ' -f 3`
	echo "$libtocopy : copying $libfile in libs"
	cp $libfile pof/libs
	patchelf --set-rpath '$ORIGIN' pof/libs/`basename $libfile`
	done

	
POF_VERSION_MAJOR=`grep "define POF_VERSION_MAJOR" ../src/version.cc | cut -d' ' -f 3`
POF_VERSION_MINOR=`grep "define POF_VERSION_MINOR" ../src/version.cc | cut -d' ' -f 3`
POF_VERSION_PATCH=`grep "define POF_VERSION_PATCH" ../src/version.cc | cut -d' ' -f 3`

if [ x$1 == xtest ] ; then
	deken package --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
else
	deken upload --no-source-error --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof
	rm -rf pof/
fi

