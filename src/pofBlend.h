/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofBlend: public pofBase {
	public:
		pofBlend(t_class *Class):
			pofBase(Class), srcFactor(GL_SRC_ALPHA), destFactor(GL_ONE_MINUS_SRC_ALPHA){  }

        virtual void draw();
        virtual void postdraw();
		static void setup(void);
		
		GLenum srcFactor;
        GLenum destFactor;
		GLenum lastSrcFactor;
		GLenum lastDestFactor;

		static GLenum currentSrcFactor;
		static GLenum currentDestFactor;
};



