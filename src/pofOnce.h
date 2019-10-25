/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofOnce;

class pofOnce : public pofBase{
	public:
		pofOnce(t_class *Class, bool continuousForce0 = false);
		~pofOnce();

		virtual void tree_draw();
		bool oncetree_process();

		bool trigger;
		bool force;
		bool final_trigger;
		bool final_force;
		bool continuousForce;
		std::list<pofOnce*> onceChildren;

		static void setup(void);
		static void initFrame(ofEventArgs & args);
		static void rebuild(ofEventArgs & args);

		static bool FORCE_ONCE;
		static std::list<pofOnce*> headOnces;
};



