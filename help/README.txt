PureData + OpenFrameworks externals

Description
-----------
Pof is a set of Pure Data (Pd) externals written with openFrameworks API,
that bring OpenGL graphics and utilities to Pd, such as :

-	basic 2D shapes drawing : rectangle, ellipse, path
-	image pre-loading, caching, modifying, saving
-	mapped 3D sphere and plane
-	anti-aliased truetype text rendering
-	translating, rotating, scaling
-	color and alpha blending management
-	shared framebuffers
-	2D multi-touch events management, taking account of the drawing order 
	of objects (the last object drawn at this place catches the event)

-	threaded rendering (so audio never waits for GUI)

-	some file utilities still missing in pd (getdir, listdir, rmfile, mkdir, rmdir...)
-	threaded file downloading from network
-	XML file parsing and editing
-	JSON file parsing (online files allowed)

The graphical interface of an application, as well as its logic and audio processing, 
thus can be entirely written as Pure Data *patches*, using Pd as an editor and real-time interpreter.

This application can then be ported as a standalone app, using ofxPd (itself 
using libpd) to a large scale of operating systems : 
Linux, OSX, Windows (untested), iOS, Android, Raspberry PI...


ofxPof is Pof, packaged as an openFrameworks addon.

[openFrameworks](http://www.openframeworks.cc) is a cross platform open source toolkit for creative coding in C++.  
[libpd](http://libpd.cc) is the Pure Data embeddable library.  
[ofxPd](https://github.com/danomatika/ofxPd) is an openFrameworks addon for adding  an instance of Pd within an openFrameworks application.  

The graphical syntax of Pof owes a lot to [Gem](http://puredata.info/downloads/gem) (however it has some noticeable differences from it).

Pof was initially developed to provide a GUI for the Android version of NinjaTune/[*Coldcut*](http://ninjatune.net/artist/coldcut)'s [Ninja Jamm](http://www.ninjajamm.com/) app. Coldcut support making the code available to the community on an open source basis.

Installation
------------

Pof can be used with Pd just like any other external; it can be downloaded from Pd menu "Help"/"Find externals" for (currently) following platforms:

*	Linux 32bit
*	Linux 64bit
*	Linux-ARM (raspberryPi)
*	OSX 32/64bit

When properly installed (the path of the library must be declared into pd prefs or 
into the application patch itself, and **pof** must be loaded with `[declare -lib pof]`)
it opens an additional window where the drawings will appear.


Documentation
-------------

This folder contains the help patches for every Pof objects. 
If this folder is declared in Pd (`-helppath path_to_ofxPof/help`) 
then the help patchs will be opened with right-click/help on Pof objects.

`example` folder contains a entry patch `pof_main.pd` which allows to launch several 
example patchs, each located in a sub-folder. 

ofxPof sources on GitHub
------------------------

https://github.com/Ant1r/ofxPof

-------
Author
-------

Copyright (c) Antoine Rousseau <antoine@metalu.net> 2014-2016  
BSD Simplified License.  
For information on usage and redistribution, and for a DISCLAIMER OF ALL WARRANTIES, see the file "LICENSE.txt" in this distribution.

