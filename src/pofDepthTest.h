/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofDepthTest;

class pofDepthTest: public pofBase {
	public:
		pofDepthTest(t_class *Class):
			pofBase(Class), enable(1.0){  }

		virtual void draw();
		virtual void postdraw();
		static void setup(void);
		
		float enable;	
};



