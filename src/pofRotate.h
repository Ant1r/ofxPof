/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

enum tAxis {X=1, Y=2, Z=0};

class pofRotate: public pofBase {
	public:
		pofRotate(t_class *Class, float a0 = 0, tAxis axis0 = Z):pofBase(Class),a(a0),axis(axis0) {  }

		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		virtual bool computeTouch(int &x, int &y);
		
		static void setup(void);
		
		float a, aReal;
		tAxis axis;
		ofVec3f axisV;
};



