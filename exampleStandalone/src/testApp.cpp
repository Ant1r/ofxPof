/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates
 */
#include "testApp.h"
#include "pofBase.h"

using namespace pd;
//--------------------------------------------------------------
void testApp::setup() {

	// the number of libpd ticks per buffer,
	// used to compute the audio buffer len: tpb * blocksize (always 64)
	#ifdef TARGET_LINUX_ARM
		// longer latency for Raspberry PI
		int ticksPerBuffer = 32; // 32 * 64 = buffer len of 2048
		int numInputs = 0; // no built in mic
	#else
		int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
		int numInputs = 1;
	#endif

	// setup OF sound stream
	ofSoundStreamSetup(2, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);

	if(!pd.init(2, numInputs, 44100, ticksPerBuffer)) {
		OF_EXIT_APP(1);
	}
	
	pofBase::setup();
	
	pd.start();
	Patch patch = pd.openPatch("pd/pof_main.pd");
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
void testApp::exit() {
	pofBase::release();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
	pofBase::touchMovedAll(x, y, 0);	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
	pofBase::touchDownAll(x, y, 0);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
	pofBase::touchUpAll(x, y, 0);
}


//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs &touch){
	pofBase::touchDownAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs &touch){
	pofBase::touchMovedAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs &touch){
	pofBase::touchUpAll(touch.x, touch.y, touch.id);
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
	pofBase::windowResized(w,h);
}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels) {
	pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
	pd.audioOut(output, bufferSize, nChannels);
}
