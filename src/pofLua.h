/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofLua: public pofBase {
	public:
		pofLua(t_class *Class);
		~pofLua();
		
		virtual void draw();
		virtual void postdraw();
		static void setup(void);

		string script;
		t_symbol *name;
};




