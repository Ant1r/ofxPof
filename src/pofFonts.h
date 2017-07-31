/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "ofxFontStash.h"

class pofFonts;

class pofFonts: public pofBase {
	public:
		pofFonts(t_class *Class, t_symbol *_font, t_symbol *_fontfile):
		 pofBase(Class),offont(NULL),font(_font),fontfile(_fontfile),need_reload(true)
		{
			fonts[font]=this;
			ofAddListener(pofBase::reloadTexturesEvent, this, &pofFonts::reloadTexture);
			ofAddListener(pofBase::unloadTexturesEvent, this, &pofFonts::unloadTexture);
		}
		~pofFonts() { 
			fonts.erase(font);
			if(offont) delete offont;
			ofRemoveListener(pofBase::reloadTexturesEvent, this, &pofFonts::reloadTexture);
			ofRemoveListener(pofBase::unloadTexturesEvent, this, &pofFonts::unloadTexture);
		}
		
		virtual void update();
		virtual bool hasUpdate(){ return true;}

		void reloadTexture(ofEventArgs & args);
		void unloadTexture(ofEventArgs & args);
		
		static void setup(void);
		
		ofxFontStash *offont;
		t_symbol *font;
		t_symbol *fontfile;

		bool need_reload;
		t_canvas *pdcanvas;
		
	static std::map<t_symbol*,pofFonts*> fonts;
	static ofxFontStash* getFont(t_symbol* font);
};



