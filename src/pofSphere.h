/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofSphere;

class pofSphere: public pofBase {
	public:
		pofSphere(t_class *Class, float rad=0, int res=0):
			pofBase(Class),radius(rad), resolution(res) {}

		virtual void draw();
		static void setup(void);
		
		float radius;
		int resolution;
		
		ofSpherePrimitive sphere;
		ofPoint textureSize;
};



