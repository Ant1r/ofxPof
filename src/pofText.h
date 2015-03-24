/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofText;

class pofText: public pofBase {
	public:
		pofText(t_class *Class, t_symbol *_font, float xanch=0, float yanch=0, float space=1, float spacing=1):
		 pofBase(Class),font(_font),xanchor(xanch), yanchor(yanch), width(0), lineHeight(0), spaceSize(space), letterSpacing(spacing) {
			m_out2 = outlet_new(&(pdobj->x_obj), 0);
		}

		virtual void draw();
		static void setup(void);
		
		t_symbol *font;
		float xanchor; // -1=left 0=center 1=right
		float yanchor; // -1=top 0=center 1=bottom
		float width, lineHeight;
		ofRectangle bound, oldBound;

		float spaceSize ; // 1='p'
		float letterSpacing; // 1= normal
		
		string str, copyStr, computedStr;		
		t_binbuf *bb;
		t_outlet *m_out2;
		
		bool mustUpdate;
		
		ofMutex mutex;
};



