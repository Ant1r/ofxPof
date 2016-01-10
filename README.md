![pOf-logo](http://metalu.net/IMG/png/pof-logo.png)
 
**P**ureData + **O**pen**F**rameworks externals


Copyright (c) Antoine Rousseau <antoine@metalu.net> 2014-2015  
BSD Simplified License.  
For information on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES, see the file "LICENSE.txt" in this distribution.

See <https://github.com/Ant1r/ofxPof> for last version.

Description
-----------
**Pof** is a set of Pure Data (Pd) *externals* written with openFrameworks API, that bring OpenGL graphics and utilities to Pd, such as :

-	basic 2D shapes drawing : rectangle, ellipse
-	image pre-loading, caching and drawing
-	mapped 3D sphere and plane
-	anti-aliased truetype text rendering
-	translating, rotating, scaling
-	color and alpha blending management
-	shared framebuffers
-	2D multi-touch events management, taking account of the drawing order of objects (the last object drawn at this place catches the event)

-	threaded rendering (so audio never waits for GUI)

-	some file utilities still missing in pd (listdir, rmfile, mkdir, rmdir...)
-	threaded file downloading from network
-	XML file parsing and editing
-	JSON file parsing (online files allowed)

Thus the graphical interface of an application, as well as its logic and audio processing, can be entirely written as Pure Data *patches*, using Pd as an editor and real-time interpreter.

This application can then be ported as a standalone app, using *ofxPd* (itself using *libpd*) to a large scale of operating systems : Linux, OSX, Windows (untested), iOS, Android, Raspberry PI...


**ofxPof** is Pof, packaged as an openFrameworks addon.

[Pure Data](http://pure-data.info) is an open source graphical patching environment.  
[openFrameworks](http://www.openframeworks.cc) is a cross platform open source toolkit for creative coding in C++.  
[ofxPd](https://github.com/danomatika/ofxPd) is an openFrameworks addon for adding  an instance of Pd within an openFrameworks application.

The graphical syntax of Pof owes a lot to [Gem](http://puredata.info/downloads/gem) (however it has some noticeable differences from it).
Pof also follows an exploration (and some enhancement) of what can be done with [PdDroidParty](http://droidparty.net/).

Pof was initially developed to provide a GUI for the Android version of NinjaTune/[*Coldcut*](http://ninjatune.net/artist/coldcut)'s [Ninja Jamm](http://www.ninjajamm.com/) app. Coldcut support making the code available to the community on an open source basis.

Usage
------

Pof can be used in two ways :

### patch edition :

Pof can be used with Pd just like any other external ; for this purpose it has to be compiled separately (see **buildExternal**(linux, 32bit only at the moment) and **OSXexternal** folders), and can be distributed as a dynamically linked library :

*	for Linux 32bit : pof.pd_linux
*	for Mac : pof.pd_darwin 
*	for Win **(untested)**: pof.dll 

When properly installed (the path of the library must be declared into pd prefs or into the application patch itself, and **pof** must be loaded, by prefs or with `[declare -lib pof]`) it opens an additional window where the drawings will appear, much like with [Gem](http://puredata.info/downloads/gem).

### patch execution :
Pof can be integrated into a standalone application, using ofxPd. Given the portability of openFrameworks, this app can be ported to various OSes, in particular mobile ones like Android or iOS, and rPI.  
This application could even be a "player" app, which loads a specific entry patch file at startup. This way updating the app just means transfering the new patch files into the device, and quit/reload the app (no compilation involved).

Some demo app projects are included here : **exampleStandalone** folder allows to build a standalone app for workstation, **PofDroid** is for building an Android Pof player, and iPof for iOS. Their entry patch are set to `bin/data/pd/pof_main.pd`.

### releases :
Check for latest binary releases [there](https://github.com/Ant1r/ofxPof/releases).

Documentation
------
The "help" folder contains the help patches for every Pof objects. If this folder is declared in Pd (`-helppath path_to_ofxPof/help`) then the help patchs will be opened with right-click/help on Pof objects.

`example/pd` folder contains a entry patch `pof_main.pd` which allows to launch several example patchs, located each in a sub-folder. This folder is copied into the `bin/data` folder of each example project (`PofDroid`, `iPof` and `exampleStandalone`), to be included into final applications.

![pofplayer](https://raw.githubusercontent.com/Ant1r/ofxPof/master/screenshots/PofPlayer.jpg) ![not_a_patch](https://raw.githubusercontent.com/Ant1r/ofxPof/master/screenshots/ThisIsNotAPatch.jpg) ![circles](https://raw.githubusercontent.com/Ant1r/ofxPof/master/screenshots/Circles.jpg) ![sampler](https://raw.githubusercontent.com/Ant1r/ofxPof/master/screenshots/Sampler.png) ![soft](https://raw.githubusercontent.com/Ant1r/ofxPof/master/screenshots/Soft.jpg)

Also there will be some Pof tutorials on [metalu.net](http://metalu.net/ressources-techniques/pure-data/pof-pd-openframeworks) one of these days.

---------

Building
--------

To compile an ofxPof project, first you need to download and install openFrameworks. Development is against the latest stable version of openFrameworks on github, which is 0.9.0 at the date of writing.

[OF github repository](https://github.com/openframeworks/openFrameworks)

On Linux, you may install Codeblocks, but using the Makefiles is just fine and simpler.

On Mac OSX, you will need to install Xcode from the Mac Developer Tools.

On Win, you will need [Codeblocks+MiniGW](http://www.codeblocks.org/downloads/26) and the [Win Codeblocks OF package](http://www.openframeworks.cc/download). Use the Codeblocks projects files with the "_win" suffix. Also, some versions of CB+MinGW are missing some libraries needed by OF (pthreads, etc). Make sure you've followed the [OF Win Codeblocks setup guide](http://openframeworks.cc/setup/codeblocks).

** warning : Codeblocks project files for windows are not ready yet. **

You also will need to install the following addons : [ofxPd](https://github.com/danomatika/ofxPd), [ofxJSON](https://github.com/jefftimesten/ofxJSON), [ofxUnicode](https://github.com/bakercp/ofxUnicode) *(***warning***: currently you have to switch to `develop` branch before downloading ofxUnicode as a ZIP)* and [ofxZipPass](https://github.com/Ant1r/ofxZipPass).
You also need [ofxAccelerometer](https://github.com/Ant1r/ofxPof/releases/download/v0.1.0/ofxAccelerometer.zip) that you will find in Android or iOS OF distribution, or you can download [here](https://github.com/Ant1r/ofxPof/releases/download/v0.1.0/ofxAccelerometer.zip).

Pof also uses [ofxFontStash](https://github.com/armadillu/ofxFontStash), but this addon is currently included into Pof (because having been hacked a bit for Android compatibility) so it doesn't have to be installed.

### Installation

Place ofxPof folder in the addons folder of the OF dir tree:
<pre>
openframeworks/addons/ofxPof
</pre>


Building as a Pd external
-------------------------

This project is located in the `buildExternal` folder.

### Linux

To build and run it on the terminal:
<pre>
make
make run</pre> (which opens `ofxPof/example/pf_main.pd`)

The final external file is : `ofxPof/buildExternal/bin/pof_pd_linux`.

### OSX

see OSXexternal README.

### Others
TODO...

Building the exampleStandalone projects
---------------------------

### Linux

To build and run it on the terminal:
<pre>
make
make run
</pre>

Optionally, you can build the example with Codeblocks : open the exampleStandalone.cbp and hit F9 to build. 

### OSX

Open the XCode project and run.

### Windows

You should be able to generate the project files with the help of OF's projectGenerator.


Building for Android
--------------------

`PofDroid` folder is a project that builds a Pof player for Android.
It has been successfully built with Eclipse under Ubuntu 12.04 / 32bit.
Follow [this link](http://openframeworks.cc/setup/android-eclipse/) to properly install Eclipse for Android.

The audio layer has been set to openSL (instead of native OF audio layer), which should give the best possible results in term of latency.

This project should compile for Android >= 4.0 (Ice Cream Sandwich, API 14) ; with some hacks of ofxAndroid (disable ofVideoPlayer and ofVideoGrabber) it can build for Android as low as 2.2 (API 8).

The size of the final APK file depends greatly of which target processors are selected. This can be set in the file `config.make`, find `ABIS_TO_COMPILE_RELEASE` variable. If you don't define it, default will build for all possible targets (arm, arm7, neon and x86), and the size of the APK will be about 23MB ; if you select only arm7 (which will cover most of the devices) it will be only about 6MB.

Building for iOS
--------------------
Open the iPof Xcode project and run.

-----------


Developing ofxPof
----------------

You can help develop ofxPof on GitHub: [https://github.com/Ant1r/ofxPof](https://github.com/Ant1r/ofxPof)

Create an account, clone or fork the repo, then request a push/merge.

If you find any bugs or suggestions please log them to GitHub as well.
