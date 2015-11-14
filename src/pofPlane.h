/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofPlane;

class pofPlane: public pofBase {
	public:
		pofPlane(t_class *Class, float w, float h):
			pofBase(Class),width(w), height(h)
		{ }

		virtual void draw();
		static void setup(void);
		
		float width, height;
};



