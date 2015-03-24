/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofBang: public pofBase {
	public:
		pofBang(t_class *Class, bool _enable):pofBase(Class), enable(_enable)
		{
			m_out2 = outlet_new(&(pdobj->x_obj), 0);
		}

		virtual void draw();
		static void setup(void);
		
		bool enable;
		t_outlet *m_out2;
};



