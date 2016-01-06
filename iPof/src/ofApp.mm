/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

#include "ofApp.h"
#include "pofBase.h"
#include "ofxAccelerometer.h"

using namespace std;
using namespace pd;

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(127, 127, 127);
    //ofSetLogLevel("Pd", OF_LOG_VERBOSE); // see verbose info inside
    
    // double check where we are ...
    cout << ofFilePath::getCurrentWorkingDirectory() << endl;
    
    // register touch events
    ofRegisterTouchEvents(this);
    
    // initialize the accelerometer
    ofxAccelerometer.setup();
    
    // iOSAlerts will be sent to this
    ofxiOSAlerts.addListener(this);
    
    // set landscape
    //ofSetOrientation(OF_ORIENTATION_90_RIGHT;
    
    // the number if libpd ticks per buffer,
    // used to compute the audio buffer len: tpb * blocksize (always 64)
    int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
    
    // setup OF sound stream
    ofSoundStreamSetup(2, 1, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    
    // setup Pd
    //
    // set 4th arg to true for queued message passing using an internal ringbuffer,
    // this is useful if you need to control where and when the message callbacks
    // happen (ie. within a GUI thread)
    //
    // note: you won't see any message prints until update() is called since
    // the queued messages are processed there, this is normal
    //

	//ofLogNotice("OF", "init pd");
	if(!puda.init(2, 1, 44100, ticksPerBuffer, false)) {
		ofExit();
	}
	
	//ofLogNotice("OF", "init pof");
	pofBase::setup();
	
	//ofLogNotice("OF", "start pd");
	puda.start();
	
	//ofLogNotice("OF", "load patch");
	Patch patch = puda.openPatch(ofToDataPath("pd/pof_main.pd"));
		
}

//--------------------------------------------------------------
void ofApp::update() {
	pofBase::updateAll();
}

//--------------------------------------------------------------
void ofApp::draw() {
	pofBase::drawAll();
}

//--------------------------------------------------------------
//void ofApp::exit() {}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs &touch){
	pofBase::touchDownAll(touch.x, touch.y, touch.id);
}

void ofApp::touchMoved(ofTouchEventArgs &touch){
	pofBase::touchMovedAll(touch.x, touch.y, touch.id);
}

void ofApp::touchUp(ofTouchEventArgs &touch){
	pofBase::touchUpAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	pofBase::windowResized(w,h);
}

//--------------------------------------------------------------
void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
	puda.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
	puda.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
/*void ofApp::reloadTextures() {
	pofBase::reloadTextures();
}

void ofApp::unloadTextures() {
	pofBase::unloadTextures();
}*/
