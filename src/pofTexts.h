/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofTexts;

class pofTexts: public pofBase {
	public:
		pofTexts(t_class *Class, t_symbol *_font, float _size, float xanch=0, float yanch=0, float spacing=0):
		 pofBase(Class),font(_font),size(_size), xanchor(xanch), yanchor(yanch), 
		 width(1e6), lineHeight(1), letterSpacing(spacing), center(false), 
		 underHeight(0), underWidth(0), underY(0),
		 maxLines(0), lineOffset(0), totalLines(0) {
			m_out2 = outlet_new(&(pdobj->x_obj), 0);
		}
		
		~pofTexts() { detach(); }
		
		virtual void draw();
		static void setup(void);
		
		t_symbol *font;
		float size;
		float xanchor; // -1=left 0=center 1=right
		float yanchor; // -1=top 0=center 1=bottom
		float width, lineHeight;
		float letterSpacing; // 1= normal
		bool center;
		float underHeight, underWidth, underY;
		int maxLines, lineOffset, totalLines;
		
		ofRectangle bound, oldBound;
		
		string str, computedStr;
		vector<string> lines;
				
		t_outlet *m_out2;
		
		bool mustUpdate;
		bool clipChanged;
		
		ofMutex mutex;
		t_canvas *pdcanvas;
};



