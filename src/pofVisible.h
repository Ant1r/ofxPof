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
		pofVisible(t_class *Class, bool v0 = false, bool fT0 = false);
		~pofVisible();

		virtual void tree_draw();
		virtual int isBlockingDraw() {return !visible;}
		
		static void setup(void);
		
		bool visible;
		bool forceTouchable;
		t_symbol *layer;
};



