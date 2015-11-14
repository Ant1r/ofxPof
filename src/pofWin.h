/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofWin: public pofBase {
	public:
		pofWin(t_class *Class);/*:pofBase(Class),init(true);{
			ofAddListener(ofEvents().windowResized,this,&pofWin::windowResized);
		}*/

		~pofWin(); /*{ 
			ofRemoveListener(ofEvents().windowResized,this,&pofWin::windowResized);
		}*/
		
		virtual void update();
		virtual bool hasUpdate(){ return true;}
		virtual bool computeTouch(int &x, int &y);
		
		void windowResized(int w, int h);

		void windowResized(ofResizeEventArgs & resize);
		/*{
			windowResized(resize.width,resize.height);
		}*/
		
		bool init;
		int r, g, b; //bg color
		
		//static :
		static void setup();
		

		static pofWin *win; // singleton master pofWin adress.
};



