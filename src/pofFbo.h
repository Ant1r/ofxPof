/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "RWmutex.h"

class pofsubFbo	{
	static std::map<t_symbol*,pofsubFbo*> sfbos;
	int refCount;
	t_symbol *name;
	public:
	float width, height;
	GLint format;
	
	ofFbo *fbo;
	
	pofsubFbo(t_symbol *n);	
	~pofsubFbo();
	
	void reloadTexture(ofEventArgs & args);
	//void unloadTexture(ofEventArgs & args);
	
	static pofsubFbo* get(t_symbol *name);			
	static void let(pofsubFbo *sfbo);
	
	void begin(float w, float h, GLint _format);
	void end();	
	void draw(float w, float h);
	void setQuality(bool quality);
};

class pofFbo: public pofBase {
	public:
		pofFbo(t_class *Class, float w=0, float h=0, GLint _format=0):
			pofBase(Class),width(w), height(h), format(_format), update(true), clear(true), quality(true) {
		}
		virtual ~pofFbo() {
			treeMutex.lockW();
			if(sfbo) pofsubFbo::let(sfbo);
			treeMutex.unlockW();
		}
		
		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		virtual void tree_draw();
		virtual int isBlockingDraw() {return (update == 0);}
		
		virtual bool computeTouch(int &x, int &y) {return true;}
		virtual bool tree_touchDown(int x, int y, int id); //select only inner touchdown
		
		static void setup(void);
		
		float width, height;
		GLint format;
		pofsubFbo *sfbo;
		bool update;
		bool clear;
		bool quality;
		ofColor tmpColor;
};



