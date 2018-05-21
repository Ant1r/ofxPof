/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates
 */
#include "ofApp.h"
#include "pofBase.h"

//#include "ofAppGLFWWindow.h"

#ifdef TARGET_LINUX_ARM
#define RASPI
#endif

#ifndef RASPI
#include "GLFW/glfw3.h"
#endif

#include "m_imp.h"
extern t_class *pofwin_class;

#ifndef RASPI
t_clock *pollEventsClock;
#endif

bool windowCreated = FALSE;

class MyThread : public ofThread {

	void threadedFunction() {
		//ofSetLogLevel(OF_LOG_VERBOSE);

	    // start
#ifdef RASPI
        ofGLESWindowSettings settings;
        settings.glesVersion = 1;
        ofCreateWindow(settings);
#else
		ofSetupOpenGL(600,300, OF_WINDOW);			// <-------- setup the GL context
#endif
		// can be OF_WINDOW or OF_FULLSCREEN
 		ofSetFrameRate(50);
        //ofGetMainLoop()->pollEvents = 0; // YOU NEED TO MAKE "void (*pollEvents)(void)" PUBLIC !!!

		string gstPluginsPath = string(pofwin_class->c_externdir->s_name) + "/libs/gstplugins";
		if(ofFile(gstPluginsPath).exists()) {
			//post("overwriting gst plugins dir: %s", gstPluginsPath.c_str());
			setenv("GST_PLUGIN_PATH", gstPluginsPath.c_str(), 1);
			setenv("GST_PLUGIN_SYSTEM_PATH", "", 1);
		}
 		ofRunApp(new ofApp());
	}

};

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
	windowCreated = TRUE;
	pofBase::drawAll();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	pofBase::keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	pofBase::keyReleased(key);
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
#ifndef RASPI
void pollEventsMethod(void* nul)
{
	if(windowCreated) {
		glfwPollEvents(); // REMOVE THE ONE IN ofAppGLFWWindow::display() !!
		clock_delay(pollEventsClock,2); //poll events every 2ms
	} else clock_delay(pollEventsClock,100);
}
#endif

extern "C" {
    /* this is called once at setup time, when this code is loaded into Pd. */
	void pof_setup(void)
	{
		(new MyThread)->startThread(true);//, true);
		pofBase::setup();

#ifndef RASPI
		pollEventsClock = clock_new(0,(t_method)pollEventsMethod);
        clock_delay(pollEventsClock,100);
#endif
	}
}


