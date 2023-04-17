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
cp bin/pof.pd_linux pof/

patchelf --set-rpath '$ORIGIN/libs' pof/pof.*

mkdir pof/libs
echo copying libs...
cp -a bin/libs/* pof/libs

LIBS_TO_COPY="freeimage boost_filesystem boost_system openal gstreamer gstbase gstapp gstvideo \
	sndfile GLEW openjpeg IlmImf IlmThread Half Iex raw gpg-error gcrypt datrie graphite2 \
	protobuf-lite mirprotobuf mircommon jbig lzma gomp lcms2 jasper jpegxr Xdmcp Xau orc \
	jxrglue pcre mirclient atspi wayland-client wayland-egl wayland-cursor Xfixes \
	openjp2 jpeg png webp webpmux ssl crypto Poco rtaudio glfw\
	"

#	gstadaptivedemux gstallocators gstbadbase libgstbadvideo gstaudio gstbadaudio gstcheck gstcodecparsers gstcontroller gstfft gstgl gstinsertbin gstmpegts gstnet gstpbutils gstphotography gstplayer gstriff gstrtp gstrtsp gsttag

for libtocopy in $LIBS_TO_COPY ; do 
	libfile=`ldd bin/pof.pd_linux | grep lib${libtocopy} | cut -d' ' -f 3`
	if [ "x$libfile" != "x" ] ; then 
		#echo "$libtocopy : copying $libfile in libs"
		cp $libfile pof/libs 
	fi
	done

echo copying gst plugins...
mkdir pof/libs/gstplugins
GSTREAMER_VERSION=0.10
RET=$(ls pof/libs/libgstreamer-1.0* &> /dev/null; echo $?)
if [ "$RET" -eq "0" ]; then
    GSTREAMER_VERSION=1.0
fi

GSTREAMER_PLUGINS=$(dirname `locate gstreamer-$GSTREAMER_VERSION/libgstavi.so`)
cp $GSTREAMER_PLUGINS/*.so* pof/libs/gstplugins

echo setting libs and plugins rpath...
for x in pof/libs/gstplugins/*.so* ; do patchelf --set-rpath '$ORIGIN/..' $x ;done
for x in pof/libs/*.so* ; do patchelf --set-rpath '$ORIGIN' $x ;done

echo copying OF install_dependencies scripts...
mkdir pof/scripts
cp -a ../../../scripts/linux/*/ pof/scripts
echo
	
POF_VERSION=`strings pof/pof.pd_linux | grep "Pof: version" | cut -s -d' ' -f 4`

rm *${POF_VERSION}*dek*

if [ x$1 == xtest ] ; then
	deken package --objects ../pof.dek.txt --version ${POF_VERSION} pof
else
	deken upload --objects ../pof.dek.txt --no-source-error --version ${POF_VERSION} pof
	rm -rf pof/
fi


