/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofIm;

class pofImage: public pofBase {
	public:
		pofImage(t_class *Class, t_symbol *f=NULL, 
			float w=0, float h=0, 
			float xa=0, float ya=0, float sx=0, float sy=0, float sw=0, float sh=0, 
			float istext=0, t_symbol *_name=NULL):
				pofBase(Class),file(NULL), displayedFile(NULL), name(_name), width(w), height(h), 
				xanchor(xa), yanchor(ya), subx(sx), suby(sy), subwidth(sw), subheight(sh), 
				image(NULL),reservedChanged(false), monitor(false),isTexture(istext!=0) 
		{
				m_out2 = outlet_new(&(pdobj->x_obj), 0);
		}
		
		~pofImage();

		void Update(); // not real "update()" !
		virtual void draw();
		virtual void postdraw();
		virtual void message(int  arc, t_atom *argv);
		
		void set(t_symbol *f);
		//void save(t_symbol *f);
		
		void reserve(t_symbol *f);
		void unreserve(t_symbol *f);
		void unreserveAll();
		
		static void setup(void);
		static void release(void);
		
		t_symbol *file, *displayedFile, *name;
		//t_symbol *savefile;
		//t_symbol *grabfbo;
		
		float width, height;	
		float imWidth, imHeight;	
		float xanchor, yanchor;
		float subx, suby;
		float subwidth, subheight;
		
		/*ofPoint resize;
		ofRectangle crop;
		ofRectangle grab;*/
		t_canvas *pdcanvas;
		pofIm *image;
		t_outlet *m_out2;
		
		std::list<pofIm*> reserved;
		bool reservedChanged;
		
		unsigned int loaderLen; // number of images waiting to be loaded.
		unsigned int loaderLenHTTP; // number of online images waiting to be loaded.
		unsigned int imgLen; // total number of (pre)loaded images.
		bool monitor;
		bool isTexture;
};



