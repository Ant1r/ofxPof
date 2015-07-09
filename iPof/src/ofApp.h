/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#include "ofxPd.h"

class ofApp : public ofxiOSApp{

	public:

		void setup();
		void update();
		void draw();
		//void exit();
		//void reloadTextures();
		//void unloadTextures();
		
		void keyPressed  (int key);
        void touchDown(ofTouchEventArgs &touch);
        void touchMoved(ofTouchEventArgs &touch);
        void touchUp(ofTouchEventArgs &touch);
    void touchDoubleTap(ofTouchEventArgs &touch){};
    void touchCancelled(ofTouchEventArgs &touch){};
		/*virtual bool backPressed(){
			ofLogNotice("testApp", "back pressed");
			ofExit();
			return true;
		}*/

		void windowResized(int w, int h);
		
		void audioReceived(float * input, int bufferSize, int nChannels);
		void audioRequested(float * output, int bufferSize, int nChannels);
		
		ofxPd puda;

};
