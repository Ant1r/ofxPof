/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofColor;

class pofColor: public pofBase {
	public:
		pofColor(t_class *Class, float r0=0, float g0=0, float b0=0, float a0=0):
			pofBase(Class),r(r0), g(g0) ,b(b0), a(a0){  }

		virtual void draw();
		virtual void postdraw();
		static void setup(void);
		
		float r, g, b, a;
		ofColor styleColor;	
};



