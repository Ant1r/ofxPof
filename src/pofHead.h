/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofHead: public pofBase {
	public:
		pofHead(t_class *Class):pofBase(Class) { /*post("new pofHead."); */pofheads.push_back(this); }
		virtual ~pofHead() { pofheads.remove(this); }
		
		virtual void draw();
		
		static void setup();
		static std::list<pofHead*> pofheads;	
};



