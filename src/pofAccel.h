/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofAccel: public pofBase {
	public:
		pofAccel(t_class *Class):pofBase(Class) {
		}
		
		virtual ~pofAccel() {
		}

		static void setup(void);
};



