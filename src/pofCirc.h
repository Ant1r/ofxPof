/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofCirc;

class pofCirc: public pofBase {
	public:
		pofCirc(t_class *Class, float w=0, float h=0, int res=0):
			pofBase(Class),width(w), height(h), resolution(res) {}

		virtual void draw();
		static void setup(void);
		
		float width, height;
		int resolution;	
};



