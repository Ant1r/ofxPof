/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofGetMatrix: public pofBase {
	public:
		pofGetMatrix(t_class *Class):pofBase(Class),trigger(false) {  
			m_out2 = outlet_new(&(pdobj->x_obj), 0); 
			m_out3 = outlet_new(&(pdobj->x_obj), 0); 
		}

		virtual void draw();
		
		static void setup(void);
		
		ofMatrix4x4 matrix;
		t_outlet *m_out2;
		t_outlet *m_out3;
		bool trigger;
};



