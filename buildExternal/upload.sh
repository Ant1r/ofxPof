#!/bin/bash

rm -rf pof/
mkdir pof/
cp -a ../help pof/
if [ -n "`uname -a | grep x86_64`" ]
	then cp bin/pof.pd_linux pof/pof.l_ia64
	else cp bin/pof.pd_linux pof/pof.l_i386
fi
	
POF_VERSION_MAJOR=`grep POF_VERSION_MAJOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_MINOR=`grep POF_VERSION_MINOR ../src/version.h | cut -d' ' -f 3`
POF_VERSION_PATCH=`grep POF_VERSION_PATCH ../src/version.h | cut -d' ' -f 3`

deken upload --version ${POF_VERSION_MAJOR}.${POF_VERSION_MINOR}.${POF_VERSION_PATCH} pof


