![pOf-logo](http://metalu.net/IMG/png/pof-logo.png)
 
**P**ureData + **O**pen**F**rameworks externals


-----------

Pof can be used with Pd just like any other external ; for this purpose it has to be compiled separately and can be distributed as a dynamically linked library :

*	for Linux 32bit : pof.l_l386
*	for Linux 32bit : pof.l_ia64
*	for Linux ARM (rasberryPi) : pof_l_arm
---------

To compile for X86-64bit, OF needs to be recompiled with -fPIC cflag, as well as all the libs that are shipped with OF. More info to come (look [there](https://github.com/Ant1r/ofxPof/releases) to get precompiled 64bit libs)...

--------

To build on the terminal:

```
make
```

The final external file is located in `bin/`.

-----

The script `upload.sh` allows to package your freshly compiled Pof external and upload it to your puredata.info account, making it available to everyone through Pd's deken system.

