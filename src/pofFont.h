/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofFont;

class pofFont: public pofBase {
	public:
		pofFont(t_class *Class, t_symbol *_font, t_symbol *_fontfile, float _size):
		 pofBase(Class),font(_font),fontfile(_fontfile),size(_size),need_reload(true)
		{
			fonts[font]=this;
		}
		~pofFont() { 
			fonts.erase(font);
		}
		
		virtual void update();
		virtual bool hasUpdate(){ return true;}
		static void setup(void);
		
		ofTrueTypeFont offont;
		t_symbol *font;
		t_symbol *fontfile;
		float size;
		bool need_reload;
		t_canvas *pdcanvas;
		
	static std::map<t_symbol*,pofFont*> fonts;
	static ofTrueTypeFont* getFont(t_symbol* font);
};



