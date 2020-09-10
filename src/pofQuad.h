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
			pofBase(Class), drawMesh(true), drawVertices(false), drawWireframe(false), drawFaces(false), needUpdate(true)
		{ 
            corners[0].set(-200, -150);
            corners[1].set(200, -150);
            corners[2].set(200, 150);
            corners[3].set(-200, 150);
            tcorners[0].set(0, 0);
            tcorners[1].set(1, 0);
            tcorners[2].set(1, 1);
            tcorners[3].set(0, 1);
            rows = columns = 40;
		}

		virtual void draw();
		static void setup(void);
		
		ofPoint tcorners[4]; // corners in texture in [0.0 ; 1.0] range
		ofPoint corners[4];
		
		int rows, columns;
		bool drawMesh;
		bool drawVertices;
		bool drawWireframe;
		bool drawFaces;
		
		ofMesh mesh;
		ofPoint textureSize;
		bool needUpdate;
		void Update();
};

