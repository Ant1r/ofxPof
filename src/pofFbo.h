/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofsubFbo;

class pofFbo: public pofBase {
	public:
		pofFbo(t_class *Class, float w=0, float h=0):pofBase(Class),width(w), height(h), update(true), clear(true) {
		}
		virtual ~pofFbo() {
		}
		
		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		virtual void tree_draw();

		virtual bool computeTouch(int &x, int &y) {return true;}
		virtual bool tree_touchDown(int x, int y, int id); //select only inner touchdown
		
		static void setup(void);
		
		float width, height;
		pofsubFbo *sfbo;
		bool update;
		bool clear;
};



