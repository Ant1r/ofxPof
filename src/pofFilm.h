/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

#ifdef TARGET_LINUX_ARM
#define RASPI
#endif

#ifdef RASPI
#include "ofxOMXPlayer.h"
#endif

class pofFilm;

class pofFilm: public pofBase {
	public:
		pofFilm(t_class *Class, float w, float h, float istext):
			pofBase(Class), 
#ifdef RASPI
	        omxplayer(NULL),
#endif
			player(NULL), width(w), height(h), playing(0), actualPlaying(0),
			currentFrame(0),
			name(NULL), file(NULL), /*loadedFile(NULL),*/isTexture(istext!=0),
			gotoFrame(-1), speed(1), useOMX(false)
		{ 
		}
		
		~pofFilm() {
			if(name) pofBase::textures.erase(name);
			if(player) {
//#ifndef RASPI
				player->stop();
//#endif
				delete player;
			}
			
#ifdef RASPI
			if(omxplayer) {
        		delete omxplayer;
			}
#endif
		}
		
		virtual void draw();
		virtual void postdraw();
		virtual void message(int  arc, t_atom *argv); // from Pd to GUI
		void load();
		
		static void setup(void);
		
#ifdef RASPI
        ofxOMXPlayer *omxplayer;
#endif
		ofVideoPlayer *player;

		float width, height, playing, actualPlaying;
		int currentFrame;
		t_symbol *name, *file;//, *loadedFile;
		bool isTexture;		
		float gotoFrame;
		float speed;
		t_canvas *pdcanvas;
		t_outlet *m_out2;
		bool useOMX; // only for Pi
};



