/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofTouchable;

class pofTouchable: public pofBase {
	public:
		pofTouchable(t_class *Class, float t0=0):pofBase(Class),touchable(t0!=0),realTouchable(t0!=0) {  }

		virtual bool tree_touchMoved(int x, int y, int id);
		virtual bool tree_touchDown(int x, int y, int id);
		virtual bool tree_touchUp(int x, int y, int id);
		virtual bool tree_touchDoubleTap(int x, int y, int id);
		virtual void tree_touchCancel();
		
		virtual bool computeTouch(int &x, int &y) {return true;}
		
		static void setup(void);
		
		bool touchable, realTouchable;	
};



