/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxPd.h"
#include "opensl_stream/opensl_stream.h"

class testApp : public ofxAndroidApp{

	public:

		void setup();
		void update();
		void draw();
		void exit();
		void reloadTextures();
		void unloadTextures();
		
		void keyPressed  (int key);
		virtual void touchDown(int x, int y, int id);
		virtual void touchMoved(int x, int y, int id);
		virtual void touchUp(int x, int y, int id);
		virtual void touchDoubleTap(int x, int y, int id) {};
		virtual void touchCancelled(int x, int y, int id) {};
		virtual bool backPressed(){
			ofLogNotice("testApp", "back pressed");
			ofExit();
			return true;
		}

		void windowResized(int w, int h);
		
		void audioReceived(float * input, int bufferSize, int nChannels);
		void audioRequested(float * output, int bufferSize, int nChannels);
		
		OPENSL_STREAM *os;
		ofxPd puda;
		static void opensl_process(void *app, int sample_rate, int buffer_frames,
			int input_channels, const short *input_buffer,
			int output_channels, short *output_buffer);

};
