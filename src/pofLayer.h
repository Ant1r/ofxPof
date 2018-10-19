/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofLayer: public pofBase {
	public:
		pofLayer(t_class *Class):
			pofBase(Class){  }

		virtual void draw();
		virtual void postdraw();
		static void setup(void);
		
		t_symbol *layer;
		t_symbol *lastLayer;
		
		static t_symbol *currentLayer;
		static void initFrame(ofEventArgs & args);
};



