/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofRect;

class pofRect: public pofBase {
	public:
		pofRect(t_class *Class, float w, float h, float tlR, float trR, float brR, float blR, float res):
			pofBase(Class),width(w), height(h), topLeftR(tlR), topRightR(trR), bottomRightR(brR), bottomLeftR(blR), resolution(res)
		{ }

		virtual void draw();
		static void setup(void);
		
		float width, height;
		float topLeftR, topRightR, bottomRightR, bottomLeftR;
		int resolution;	//* resolution for rounded
};



