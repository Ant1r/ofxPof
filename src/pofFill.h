/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofFill: public pofBase {
	public:
		pofFill(t_class *Class):
			pofBase(Class), fill(OF_FILLED), lineWidth(0.0){  }

        virtual void draw();
        virtual void postdraw();
		static void setup(void);
		
		ofFillFlag fill;
		ofFillFlag lastFill;
		float lineWidth;
		float lastLineWidth;
		
		static float currentLineWidth;
};



