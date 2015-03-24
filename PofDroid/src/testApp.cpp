/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

#include "testApp.h"
#include "pofBase.h"
#include "ofxAccelerometer.h"

using namespace std;
using namespace pd;

//--------------------------------------------------------------
void testApp::setup() {

	// the number of libpd ticks per buffer,
	// used to compute the audio buffer len: tpb * blocksize (always 64)
	//#ifdef TARGET_LINUX_ARM
		// longer latency for Raspberry PI
		//int ticksPerBuffer = 32; // 32 * 64 = buffer len of 2048
		//int numInputs = 0; // no built in mic
	//#else
		int ticksPerBuffer = 1; // 8 * 64 = buffer len of 512
		int numInputs = 1;
	//#endif
	
    ofSetFrameRate(60);
	ofSetVerticalSync(true);

	ofLogNotice("OF", "init sound");
	// setup OF sound stream
	//ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 4);
	os = NULL;
	os = opensl_open(44100, numInputs, 2, ticksPerBuffer*PdBase::blockSize(), testApp::opensl_process, (void*)this);

	ofxAccelerometer.setup();
	
	ofLogNotice("OF", "init pd");
	if(!puda.init(2, numInputs, 44100, ticksPerBuffer)) {
		ofExit();
	}
	
	ofLogNotice("OF", "init pof");
	pofBase::setup();
	
	ofLogNotice("OF", "start pd");
	puda.start();
	
	ofLogNotice("OF", "load patch");
	Patch patch = puda.openPatch(ofToDataPath("pd/pof_main.pd"));
		
	if(os) opensl_start(os);
}

//--------------------------------------------------------------
void testApp::update() {
	pofBase::updateAll();
}

//--------------------------------------------------------------
void testApp::draw() {
	pofBase::drawAll();
}

//--------------------------------------------------------------
void testApp::exit() {}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {}

//--------------------------------------------------------------
void testApp::touchDown(int x, int y, int id){
	pofBase::touchDownAll(x, y, id);
}

void testApp::touchMoved(int x, int y, int id){
	pofBase::touchMovedAll(x, y, id);	
}

void testApp::touchUp(int x, int y, int id){
	pofBase::touchUpAll(x, y, id);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
	pofBase::windowResized(w,h);
}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels) {
	puda.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
	puda.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::reloadTextures() {
	pofBase::reloadTextures();
}

void testApp::unloadTextures() {
	pofBase::unloadTextures();
}

//--------------------------------------------------------------
short testInBuf[1024], testOutBuf[1024];
void testApp::opensl_process(void *app, int sample_rate, int buffer_frames,
			int input_channels, const short *input_buffer,
			int output_channels, short *output_buffer) {
	((testApp*)app)->puda.PdBase::processShort(/*buffer_frames*/1,(short *)input_buffer, output_buffer);
}
