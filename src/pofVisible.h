/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofTouchable.h"

class pofVisible;

class pofVisible: public pofTouchable {
	public:
		pofVisible(t_class *Class, bool v0 = false, bool fT0 = false, bool once0 = false, bool continuousForce0 = false);
		~pofVisible();

		virtual void tree_draw();
		
		static void setup(void);
		static void initFrame(ofEventArgs & args);
		
		bool visible;
		bool forceTouchable;
		bool once;
		bool continuousForce;
		bool force;
		bool final_once;
		bool final_force;
		t_symbol *layer;
		static bool FORCE_ONCE;
		static std::list<pofVisible*> onces;
};



