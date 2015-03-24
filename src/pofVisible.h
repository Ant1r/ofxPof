/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofTouchable.h"

class pofVisible;

class pofVisible: public pofTouchable {
	public:
		pofVisible(t_class *Class, float v0=0):pofTouchable(Class, v0),visible(v0) {  }

		virtual void tree_draw();
		
		static void setup(void);
		
		float visible;
};



