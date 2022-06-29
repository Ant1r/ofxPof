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
	public:
	t_pd pd;
	pofLua *lua;
	set<t_symbol*> channels;
	pofLua_receiver(pofLua *owner);
	~pofLua_receiver();
	void bind(t_symbol *s);
	void unbind(t_symbol *s);
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
		pofLua_receiver receiver;
		pofLua_receiver receiverupdater;

		static void setup(void);
};




