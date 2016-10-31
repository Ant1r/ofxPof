![pOf-logo](http://metalu.net/IMG/png/pof-logo.png)
 
**P**ureData + **O**pen**F**rameworks externals

Description
-----------
**Pof** is a set of Pure Data (Pd) *externals* written with openFrameworks API, that bring OpenGL graphics and utilities to Pd, such as :

-	basic 2D shapes drawing : rectangle, ellipse, path
-	image pre-loading, caching, modifying, saving
-	mapped 3D sphere and plane
-	anti-aliased truetype text rendering
-	translating, rotating, scaling
-	color and alpha blending management
-	shared framebuffers
-	2D multi-touch events management, taking account of the drawing order of objects (the last object drawn at this place catches the event)

-	threaded rendering (so audio never waits for GUI)

-	some file utilities still missing in pd (getdir, listdir, rmfile, mkdir, rmdir...)
-	threaded file downloading from network
-	XML file parsing and editing
-	JSON file parsing (online files allowed)

The graphical interface of an application, as well as its logic and audio processing, thus can be entirely written as Pure Data *patches*, using Pd as an editor and real-time interpreter.

This application can then be ported as a standalone app, using *ofxPd* (itself using *libpd*) to a large scale of operating systems : Linux, OSX, Windows (untested), iOS, Android, Raspberry PI...


**ofxPof** is Pof, packaged as an openFrameworks addon.

[Pure Data](http://pure-data.info) is an open source graphical patching environment.  
[openFrameworks](http://www.openframeworks.cc) is a cross platform open source toolkit for creative coding in C++.  
[libpd](http://libpd.cc) is the Pure Data embeddable library.  
[ofxPd](https://github.com/danomatika/ofxPd) is an openFrameworks addon for adding  an instance of Pd within an openFrameworks application.  

The graphical syntax of Pof owes a lot to [Gem](http://puredata.info/downloads/gem) (however it has some noticeable differences from it).
Pof also follows an exploration (and some enhancement) of what can be done with [PdDroidParty](http://droidparty.net/).

Pof was initially developed to provide a GUI for the Android version of NinjaTune/[*Coldcut*](http://ninjatune.net/artist/coldcut)'s [Ninja Jamm](http://www.ninjajamm.com/) app. Coldcut support making the code available to the community on an open source basis.

Usage
------

Pof can be used in two ways :

### patch edition :

Pof can be used with Pd just like any other external; it can be downloaded from Pd menu "Help"/"Find externals" for (currently) following platforms:

*	Linux 32bit
*	Linux 64bit
*	Linux-ARM (raspberryPi)
*	OSX 32/64bit

Look at [linuxExternal](./linuxExternal) and [OSXexternal](./OSXexternal) folders for manual compilation.

When properly installed (the path of the library must be declared into pd prefs or into the application patch itself, and **pof** must be loaded, by prefs or with `[declare -lib pof]`) it opens an additional window where the drawings will appear, much like with [Gem](http://puredata.info/downloads/gem).

### patch execution :
Pof can be integrated into a standalone application, using ofxPd. Given the portability of openFrameworks, this app can be ported to various OSes, in particular mobile ones like Android or iOS, and rPI.  
This application could even be a "player" app, which loads a specific entry patch file at startup. This way updating the app just means transfering the new patch files into the device, and quit/reload the app (no compilation involved).

Some demo app projects are included here : [exampleStandalone](./exampleStandalone) folder allows building a standalone app for workstation, [PofDroid](./PofDroid) is for building a Pof player for Android, and [iPof](./iPof) for iOS. Their entry patch are set to `bin/data/pd/pof_main.pd`, which can be changed in the sources.


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

You also will need to install the following addons : [ofxPd](https://github.com/danomatika/ofxPd), [ofxJSON](https://github.com/jefftimesten/ofxJSON) and [ofxZipPass](https://github.com/Ant1r/ofxZipPass).
You also need [ofxAccelerometer](https://github.com/Ant1r/ofxPof/releases/download/v0.1.0/ofxAccelerometer.zip) that you will find in Android or iOS OF distribution, or you can download [here](https://github.com/Ant1r/ofxPof/releases/download/v0.1.0/ofxAccelerometer.zip).

Pof also uses [ofxFontStash](https://github.com/armadillu/ofxFontStash) and its dependency [ofxUnicode](https://github.com/bakercp/ofxUnicode), but these addons have been included into Pof (for compilation ease and because having been hacked a bit for Android compatibility) so don't have to be installed.

### Installation

Place ofxPof folder in the addons folder of the OF dir tree:
<pre>
openframeworks/addons/ofxPof
</pre>




Developing ofxPof
----------------

You can help develop ofxPof on GitHub: [https://github.com/Ant1r/ofxPof](https://github.com/Ant1r/ofxPof)

Create an account, clone or fork the repo, then request a push/merge.

If you find any bugs or suggestions please log them to GitHub as well.


Author
-------

Copyright (c) Antoine Rousseau <antoine@metalu.net> 2014-2016  
BSD Simplified License.  
For information on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES, see the file "LICENSE.txt" in this distribution.

See <https://github.com/Ant1r/ofxPof> for last version.
