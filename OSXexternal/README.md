### Pof OSXexternal

Here is the Xcode project to build Pof as a Pd external for OSX, allowing to edit Pof patches with Pd on OSX.

**notice** : to build OSX pof external you need to hack openFrameworks source a bit :   
in the file `of_v0.9.0_osx_release/libs/openFrameworks/app/ofMainLoop.h`, move the declaration :  
`void (*pollEvents)(void);`  
to the public area, for example at the end of this area, after the line :  
`ofEvent<void> exitEvent;`


