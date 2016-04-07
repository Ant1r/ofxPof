/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofQuad;

class pofQuad: public pofBase {
	public:
		pofQuad(t_class *Class):
			pofBase(Class), drawMesh(true), drawVertices(false), drawWireframe(false), drawFaces(false)
		{ 
            corners[0].set(-200,-150);
            corners[1].set(200,-150);
            corners[2].set(200,150);
            corners[3].set(-200,150);
            rows = columns = 40;
		}

		virtual void draw();
		static void setup(void);
		
		ofPoint corners[4];
		int rows, columns;
		bool drawMesh;
		bool drawVertices;
		bool drawWireframe;
		bool drawFaces;
};

