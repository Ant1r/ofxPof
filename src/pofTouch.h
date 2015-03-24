/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofTouch;

class pofTouch: public pofBase {
	public:
		pofTouch(t_class *Class, float w=0, float h=0, float dont_capture=0,float multi_=0):
			pofBase(Class), width(w), height(h), do_draw(false), capture(dont_capture==0),
			dynamic(dont_capture==2), multi(multi_), active_id(-1), inside(false), ellipse(false){
				std::fill_n(captured, 32, false);
				m_out2 = outlet_new(&(pdobj->x_obj), 0);
			}

		virtual void draw();
		static void setup(void);
		virtual bool touchMoved(int x, int y, int id);
		virtual bool touchDown(int x, int y, int id);
		virtual bool touchUp(int x, int y, int id);
		virtual bool touchDoubleTap(int x, int y, int id);
		virtual void touchCancel();
		virtual bool isTouchable() {return true;}
		
		bool isInside(int x,int y);
		void Send(t_symbol *s, int n, float f1, float f2=0, float f3=0); // outlet_anything
		
		float width, height;
		bool do_draw;
		bool capture;
		bool dynamic; // dynamically test enter/leave event. Implies capture = 0. (true if dont_capture==2)
		bool multi;
		int active_id;
		bool captured[32];
		bool inside;
		
		bool ellipse;
		
		t_outlet *m_out2;
		
		static t_symbol *s_enter, *s_leave, *s_down, *s_up, *s_move, *s_cancel; 
};



