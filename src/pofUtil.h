/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofFileDownloader;
class pofUnzipper;
class pofSysThread;

class pofUtil: public pofBase {
	public:
		pofUtil(t_class *Class):pofBase(Class),fileDownloader(NULL), unzipper(NULL), systhread(NULL) {
		}
				
		static void setup(void);
		
		t_canvas *pdcanvas;
		int pargc; 		//parent canvas arguments count
		t_atom *pargv; 	//parent canvas arguments
		
		pofFileDownloader *fileDownloader;
		pofUnzipper *unzipper;
		pofSysThread *systhread;
};



