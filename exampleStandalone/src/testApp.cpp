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
	std::vector<string>::iterator it; // for parsing commandline arguments

		int numOutputs = 2; 
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

	// ----------- parse arguments for audio parameters --------------
	it = find (arguments.begin(), arguments.end(), "-inchannels");
    if(it != arguments.end()) {
        it++;
        if(it != arguments.end()) {
            numInputs = (int)std::stol(*it);
            ofLogNotice("Requested audio input channels :") << numInputs;
        }
    }
    
	it = find (arguments.begin(), arguments.end(), "-outchannels");
    if(it != arguments.end()) {
        it++;
        if(it != arguments.end()) {
            numOutputs = (int)std::stol(*it);
            ofLogNotice("Requested audio output channels :") << numOutputs;
        }
    }

	// ----------- setup OF sound stream --------------
	// unfortunately the following trick doesn't work, nor not to initialize audio at all. Which other way to disable audio ?... 
	/*it = find (arguments.begin(), arguments.end(), "-noaudio");
	if(it != arguments.end()) ofSoundStreamSetup(0, 0, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);
    else*/
    ofSoundStreamSetup(numOutputs, numInputs, this, 44100, ofxPd::blockSize()*ticksPerBuffer, 3);

	if(!pd.init(numOutputs, numInputs, 44100, ticksPerBuffer)) {
		OF_EXIT_APP(1);
	}
	
	pofBase::setup();
	
	pd.start();
		it = find (arguments.begin(), arguments.end(), "-hidecursor");
	if(it != arguments.end()) ofHideCursor();
	
	it = find (arguments.begin(), arguments.end(), "-open");
	if( (it != arguments.end()) && ((++it) != arguments.end()) )
		Patch patch = pd.openPatch(*it);
	else Patch patch = pd.openPatch("pd/pof_main.pd");
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
