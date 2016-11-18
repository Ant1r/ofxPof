/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates
 */
#include "ofApp.h"
#include "pofBase.h"

class MyThread : public ofThread {
 
	void threadedFunction() {
		//ofSetLogLevel(OF_LOG_VERBOSE);

	    // start
		/*ofAppGLFWWindow win;  
		win.setMultiDisplayFullscreen(true); //this makes the fullscreen window span across all your monitors  

		ofSetupOpenGL(&win, 600,300, OF_WINDOW);  

//		ofSetupOpenGL(600,300, OF_WINDOW);			// <-------- setup the GL context*/

		// can be OF_WINDOW or OF_FULLSCREEN
		ofGLFWWindowSettings settings;

		settings.multiMonitorFullScreen = true;
		settings.windowMode = OF_WINDOW;
		settings.width = 600;
		settings.height = 300;
		ofCreateWindow(settings);

 		ofSetFrameRate(50);
 		ofRunApp(new ofApp());
	} 
 
};

MyThread *GUIthread;
//--------------------------------------------------------------
void ofApp::exit(){
	
}

//--------------------------------------------------------------
void ofApp::setup(){
	//ofRegisterTouchEvents(this);
	ofSetEscapeQuitsApp(false);	
}

//--------------------------------------------------------------
void ofApp::update(){
	if(pofBase::watchdogCount > 10) { // watchdog test. not working... (pd still hangs when quit)
//		pofBase::release();
//		ofExit();
		ofLogWarning("pof", "watchdog !");
		pofBase::watchdogCount = 0;
	}
	pofBase::updateAll();
}

//--------------------------------------------------------------
void ofApp::draw(){
	pofBase::drawAll();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	post("keyPressed %d", key);
	if(key == 113) { // 'q'
		pofBase::release();
		post("pofBase released");
		ofExit();
		post("Pof exited");
		GUIthread->waitForThread(true);
		post("Pof thread stopped");
		ofGetWindowPtr()->close();
		post("window closed");
		(GUIthread=new MyThread)->startThread(true);//, true);
		post("pof restarted");
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
	//post("mouse_moved %d %d", x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	pofBase::touchMovedAll(x, y, 0);	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	pofBase::touchDownAll(x, y, 0);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	pofBase::touchUpAll(x, y, 0);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	pofBase::windowResized(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::touchDown(ofTouchEventArgs &touch){
	pofBase::touchDownAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs &touch){
	pofBase::touchMovedAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs &touch){
	pofBase::touchUpAll(touch.x, touch.y, touch.id);
}

//--------------------------------------------------------------
/*void ofApp::touchDoubleTap(ofTouchEventArgs &touch){
}*/

extern "C" {
    /* this is called once at setup time, when this code is loaded into Pd. */
	void pof_setup(void)
	{
		(GUIthread=new MyThread)->startThread(true);//, true);
		pofBase::setup();
	}
}


