/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "IPVideoGrabber.h"

#ifdef TARGET_LINUX_ARM
#define RASPI
#endif

class pofIPcam: public pofBase {
	public:
		pofIPcam(t_class *Class, float w, float h, float istext):
			pofBase(Class), 
			width(w), height(h), isConnected(false),
			name(NULL), url(NULL), isTexture(istext!=0)
		{
		}
		
		~pofIPcam() {
            cam.waitForDisconnect();
            if(name) pofBase::textures.erase(name);
        }
		
		virtual void draw();
		virtual void postdraw();
		virtual void message(int  arc, t_atom *argv); // from Pd to GUI
		void load();
		
		static void setup(void);
		
		ofx::Video::IPVideoGrabber cam;

        float width, height;
        bool isConnected;
		t_symbol *name, *url;
		bool isTexture;		
		t_canvas *pdcanvas;
		t_outlet *m_out2;
};



