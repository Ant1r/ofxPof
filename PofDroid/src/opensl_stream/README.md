opensl_stream
=============

A simple callback-driven audio API for Android that sits on top of OpenSL and provides a credible illusion of synchronized input and output.

Make sure to include "-lOpenSLES -llog" in LOCAL_LDLIBS.

This library only requires Android 2.3 or later, i.e., it will work on all Android devices that support OpenSL.

Sample code, including Java code for properly configuring OpenSL: https://github.com/nettoyeurny/opensl_stream_sample

Important
---------

Internally, OpenSL operates at a device-dependent native sample rate and buffer size. If at all possible, you should have opensl_stream operate at the native sample rate and buffer size, or else you'll risk suboptimal performance or even glitches.

Jelly Bean MR1 introduced an API for detecting the native sample rate and buffer size; the sample project for opensl_stream shows how to use it. If you're targeting older devices, or if your synthesis library will not work at the native buffer size, then a sample rate of 44100Hz and a buffer size of 64 frames is a reasonable default. When in doubt, choose a smaller buffer size; larger buffers do not improve stability.

Known issues
------------

* At 48000Hz, Galaxy Nexus and Nexus 10 produce glitchy output.
* At 44100Hz, Galaxy Nexus tends to glitch when switching activities or bringing up large dialogs.
* Touch sounds occasionally cause OpenSL to glitch. It's probably a good idea to disable touch sounds in audio apps.

These problems are not specific to opensl_stream and have been reproduced in other settings.
