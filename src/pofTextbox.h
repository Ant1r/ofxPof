/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#ifdef TARGET_ANDROID
#include <ofxAndroidUtils.h>
#endif

class pofTextbox: public pofBase {
	public:
		pofTextbox(t_class *Class): pofBase(Class), question(""), answer(""), needShow(false), needSend(false){
#ifdef TARGET_ANDROID
			ofAddListener(ofxAndroidEvents().okPressed,this,&pofTextbox::okPressed);
			ofAddListener(ofxAndroidEvents().cancelPressed,this,&pofTextbox::cancelPressed);
#endif
		}
		virtual ~pofTextbox() {
#ifdef TARGET_ANDROID
			ofRemoveListener(ofxAndroidEvents().okPressed,this,&pofTextbox::okPressed);
			ofRemoveListener(ofxAndroidEvents().cancelPressed,this,&pofTextbox::cancelPressed);
#endif
		}
		
		virtual void update();
		virtual bool hasUpdate(){ return true;}
#ifdef TARGET_ANDROID
		virtual void okPressed(bool &b);
		virtual void cancelPressed(bool &b);
#endif
		static void setup(void);
		
		t_binbuf *bb;
		string question;
		string defAnswer;
		string answer;
		bool needShow;
		bool needSend;
};



