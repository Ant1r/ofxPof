/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofTouch.h"

class pofLua: public pofTouch {
	public:
		pofLua(t_class *Class);
		virtual ~pofLua();
		
		virtual void draw();
		virtual void postdraw();
		virtual void Send(t_symbol *s, int n, float f1, float f2=0, float f3=0); // outlet_anything
		virtual bool isTouchable() {return touchable;}
		string script;
		t_symbol *name;
		t_symbol *filename;
		t_canvas *pdcanvas;
		bool loaded;
		bool touchable;
		bool drawable;

		static void setup(void);
};




