/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofScale;

class pofScale: public pofBase {
	public:
		pofScale(t_class *Class, float x0=0, float y0=0):pofBase(Class),v(x0,y0) {  }

		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		virtual bool computeTouch(int &x, int &y);
		
		static void setup(void);
		
		ofVec2f v, vReal;
};



