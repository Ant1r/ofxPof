#!/bin/bash

rm -rf pof/
mkdir pof/

echo ; echo copying help patches...
cp -a ../help/* pof/
cp ../LICENSE.txt pof/

echo ; echo copying example patches...
mkdir pof/example/
cp -a ../example/pd/* pof/example/


echo ; echo copying pof external...
cp bin/pof.* pof/

patchelf --set-rpath '$ORIGIN/libs' pof/pof.*

mkdir pof/libs
echo ; echo copying libs...
cp -a bin/libs/* pof/libs

LIBS_TO_COPY="freeimage boost_filesystem boost_system openal gstreamer gstbase gstapp gstvideo \
	sndfile GLEW openjpeg IlmImf IlmThread Half Iex raw gpg-error gcrypt datrie graphite2 \
	protobuf-lite mirprotobuf mircommon jbig lzma gomp lcms2 jasper jpegxr Xdmcp Xau orc \
	jxrglue pcre mirclient atspi wayland-client wayland-egl bwayland-cursor Xfixes openjp2"

for libtocopy in $LIBS_TO_COPY ; do 
	libfile=`ldd bin/pof.* | grep lib${libtocopy} | cut -d' ' -f 3`
	echo "$libtocopy : copying $libfile in libs"
	cp $libfile pof/libs
	patchelf --set-rpath '$ORIGIN' pof/libs/`basename $libfile`
	done

echo ; echo copying OF install_dependencies scripts...
mkdir pof/scripts
cp -a ../../../scripts/linux/*/ pof/scripts
echo
	
POF_VERSION=`strings pof/pof.* | grep "Pof: version" | cut -s -d' ' -f 4`

if [ x$1 == xtest ] ; then
	deken package --version ${POF_VERSION} pof
else
	deken upload --no-source-error --version ${POF_VERSION} pof
	rm -rf pof/
fi


