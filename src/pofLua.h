/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofTouch.h"
#include "pofOnce.h"

class pofLua;

class pofLua_receiver
{
	t_pd pd;
	pofLua *lua;
	bool update;
	t_symbol *name;
	public:
	pofLua_receiver();
	~pofLua_receiver();
	void initialize(pofLua *owner, t_symbol *name, bool update = false);
	void rcv_anything(t_symbol *s, int argc, t_atom *argv);
	static void setup(void);
};

class pofLua: public pofTouch, public pofOnce {
	public:
		pofLua(t_class *Class);
		virtual ~pofLua();
		
		virtual void draw();
		virtual void postdraw();
		virtual void Send(t_symbol *s, int n, float f1, float f2=0, float f3=0); // outlet_anything
		virtual bool isTouchable() {return touchable;}
		virtual void message(int  arc, t_atom *argv); // Pd -> Pof(lua)
		string script;
		string argsScript;
		t_symbol *name;
		t_symbol *filename;
		t_canvas *pdcanvas;
		bool loaded;
		bool touchable;
		bool drawable;
		map<t_symbol*, pofLua_receiver> receivers;

		static void setup(void);
};




