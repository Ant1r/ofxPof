/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "ofMain.h"
#include "m_pd.h"
#include <Poco/Any.h>
using namespace Poco;

class pofBase;

typedef struct _PdObject {
	t_object x_obj;
	pofBase* parent;
	t_float x_f;
} PdObject;

class pofBase {
	public:
		pofBase(t_class *Class) { 
			lock();
			char selfname[16];

			pdobj = (PdObject*)pd_new(Class);
			pdobj->parent = (pofBase*) this;
			
			snprintf(selfname,16 , "pof%p", (void*)this);
			s_self = gensym(selfname);
			pd_bind(&pdobj->x_obj.ob_pd, s_self);
			
			m_out1 = outlet_new(&(pdobj->x_obj), 0);
			
			pofobjs.push_back(this);
			needBuild = true;
			
			tmpToGUIclock = clock_new(&(pdobj->x_obj), (t_method)tryQueueTmpToGUI);
			
			unlock();
		}
		
		virtual ~pofBase() { 
			lock();
			pd_unbind(&pdobj->x_obj.ob_pd, s_self);
			pofobjs.remove(this); 
			if (m_out1) outlet_free(m_out1);
			needBuild = true;
			unlock();
		}

		void detach() {
			lock();
			pofobjs.remove(this); 
			needBuild = true;
			unlock();
		}
			
		virtual void update() {}
		virtual bool hasUpdate(){ return false;}
		virtual void draw() {}
		virtual void postdraw() {} // called after objects bellow have been drawn
		virtual void message(int argc, t_atom *argv) {} // process incoming message from Pd side
		
		//virtual void keyPressed(int key);
		//virtual void keyReleased(int key);
		//virtual void mouseMoved(int x, int y)  {}
		virtual bool computeTouch(int &x, int &y) {return false;}
		virtual bool isTouchable() {return false;}
		virtual bool touchMoved(int x, int y, int id) {return false;}
		virtual bool touchDown(int x, int y, int id) {return false;}
		virtual bool touchUp(int x, int y, int id) {return false;}
		virtual bool touchDoubleTap(int x, int y, int id) {return false;}
		virtual void touchCancel() {}
		//void dragEvent(ofDragInfo dragInfo);
		//void gotMessage(ofMessage msg);

		
		virtual void tree_update();
		virtual void tree_draw();
		virtual bool tree_touchMoved(int x, int y, int id);
		virtual bool tree_touchDown(int x, int y, int id);
		virtual bool tree_touchUp(int x, int y, int id);
		virtual bool tree_touchDoubleTap(int x, int y, int id);
		virtual void tree_touchCancel();
		
		void reset_tree();
		
		void tree_build(pofBase* parent);
		bool touchtree_build(pofBase* parent);
		
		// Messaging :
		void queueToSelfPd(int argc, t_atom *argv); // prefix by selfname then queue to pd.
		void queueToGUI(t_symbol *s, int argc, t_atom *argv); // queue to tmpGUI, then call tryQueueTmpToGUI.
		static void tellGUI(void* x, t_symbol *s, int argc, t_atom *argv) // ready to use in class_addmethod
		{
	    pofBase* px = (pofBase*)(((PdObject*)x)->parent);
      px->queueToGUI(s, argc, argv);
    }

		static void tryQueueTmpToGUI(void *x); // try_lock toGUImutex then copy tmpQueue to toGUIQueue, 
		                                       // else retry after delay(0).
		t_binbuf *dequeueToGUI(); // to be used by GUI; returns the next binbuf in the queue (null if none);
		                          // caller must free the binbuf after use.
		
		// data :
		
		PdObject *pdobj;
		t_outlet *m_out1;
		bool isBuilt;		
		std::list<pofBase*> children, touchChildren;
		t_symbol *s_self;
		t_clock *tmpToGUIclock;
		deque<t_binbuf*> toGUIQueue, tmpToGUIQueue;
		ofMutex toGUImutex;
		
		// static :
		
		static std::list<pofBase*> pofobjs;
		static ofMutex globMutex;		
		static bool needBuild;
		static ofEvent<ofEventArgs> reloadTexturesEvent, unloadTexturesEvent;
		static deque<t_binbuf*> toPdQueue;
		static deque<std::vector<Any> > toPdQueueVec;
		static t_clock *queueClock;
		static bool doRender;
		
		static ofTexture *currentTexture;
		
		static void pof_build(void *x, t_symbol *s, int argc, t_atom *argv);
		
		static void buildAll();
		static void updateAll();
		static void drawAll();
		static void lock() {globMutex.lock();}
		static void unlock(){globMutex.unlock();}
		
		static void touchDownAll(int x, int y, int id);
		static void touchMovedAll(int x, int y, int id);
		static void touchUpAll(int x, int y, int id);
		//static void touchDoubleTapAll(int x, int y, int id); // if you want double tap, do it in the patch ;-)
		static void touchCancelAll();

		static void windowResized(int w, int h);
		
		static void setup(); // Initialize pd external classes.
		static void release(); // Release resources before exiting.
		
		static void reloadTextures();
		static void unloadTextures();
		
		static void sendToPd(t_binbuf *bb);
		static void sendToPd(std::vector<Any> &vec);

		static bool dequeueToPd();
		static bool dequeueToPdVec();
		static void backPressed();
		
		static void setString(const string& id, const string& value);
		static string getString(const string& id);
};

t_symbol *makefilename(t_symbol *f, t_canvas *pdcanvas);


#define POF_SETUP(CLASS) \
	class_addmethod(CLASS, (t_method)pofBase::pof_build, gensym("pof_build"), A_GIMME, A_NULL);



