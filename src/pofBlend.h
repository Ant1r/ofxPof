/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofBlend: public pofBase {
	public:
		pofBlend(t_class *Class, float v0=0):pofBase(Class) {  }

        virtual void draw();
        virtual void postdraw();
		static void setup(void);
		
		int srcFactor;
        int destFactor;
};



