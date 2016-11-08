/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofPath;

class pofPath: public pofBase {
	public:
		pofPath(t_class *Class):
			pofBase(Class)
		{ }

		ofPath path;

		virtual void draw();
		virtual void message(int  arc, t_atom *argv);
		static void setup(void);
};



