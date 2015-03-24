/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofResetMatrix;

class pofResetMatrix: public pofBase {
	public:
		pofResetMatrix(t_class *Class):pofBase(Class),hasMatrix(false) {  }

		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		
		static void setup(void);

		ofMatrix4x4 matrix;
		bool hasMatrix;
};



