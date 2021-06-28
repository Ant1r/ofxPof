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
		 pofBase(Class),offont(NULL),font(_font),fontfile(_fontfile),need_reload(true), scale(1.0)
		{
			fonts[font]=this;
			ofAddListener(pofBase::reloadTexturesEvent, this, &pofFonts::reloadTexture);
			ofAddListener(pofBase::unloadTexturesEvent, this, &pofFonts::unloadTexture);
		}
		~pofFonts() { 
			fonts.erase(font);
			if(offont) {
				offontsToDelete.push_back(offont);
			}
			ofRemoveListener(pofBase::reloadTexturesEvent, this, &pofFonts::reloadTexture);
			ofRemoveListener(pofBase::unloadTexturesEvent, this, &pofFonts::unloadTexture);
		}

		virtual void update();
		virtual bool hasUpdate(){ return true;}

		void reloadTexture(ofEventArgs & args);
		void unloadTexture(ofEventArgs & args);

		ofxFontStash *offont;
		t_symbol *font;
		t_symbol *fontfile;
		bool need_reload;
		t_canvas *pdcanvas;
		float scale;

		static void setup(void);
		static pofFonts* getFont(t_symbol* font);
		static void initFrame(ofEventArgs & args){
			while(!offontsToDelete.empty()) {
				delete offontsToDelete.front();
				offontsToDelete.pop_front();
			}
		}

		static std::map<t_symbol*, pofFonts*> fonts;
		static std::list<ofxFontStash*> offontsToDelete;

};



