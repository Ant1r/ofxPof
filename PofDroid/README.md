Building for Android
--------------------

This folder builds a Pof player for Android.
It has been successfully built with Eclipse under Ubuntu 12.04 / 32bit.
Follow [this link](http://openframeworks.cc/setup/android-eclipse/) to properly install Eclipse for Android.

The audio layer has been set to openSL (instead of native OF audio layer), which should give the best possible results in term of latency.

This project should compile for Android >= 4.0 (Ice Cream Sandwich, API 14) ; with some hacks of ofxAndroid (disable ofVideoPlayer and ofVideoGrabber) it can build for Android as low as 2.2 (API 8).

The size of the final APK file depends greatly of which target processors are selected. This can be set in the file `config.make`, find `ABIS_TO_COMPILE_RELEASE` variable. If you don't define it, default will build for all possible targets (arm, arm7, neon and x86), and the size of the APK will be about 23MB ; if you select only arm7 (which will cover most of the devices) it will be only about 6MB.
