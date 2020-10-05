/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates
 */
#include "ofApp.h"
#include "pofBase.h"
#include "pofWin.h"
//#include "ofAppGlutWindow.h"
#include "ofAppGLFWWindow.h"

#if (_MSC_VER)
#include <GLFW/glfw3.h>
#else
#include "GLFW/glfw3.h"
#endif

t_clock *pollEventsClock;

bool windowCreated = false;
bool windowNoBorder = false;

extern const struct {
    unsigned int 	 width;
    unsigned int 	 height;
    unsigned int 	 bytes_per_pixel; /* 3:RGB, 4:RGBA */
    unsigned char	 pixel_data[56 * 80 * 4 + 1];
} pointer56;

ofImage pointerImage;

class MyThread : public ofThread {
    
    void threadedFunction() {
        ofSetLogLevel(OF_LOG_SILENT);
        
        // start
        //ofLogNotice("pof start thread");
        
        //ofAppGlutWindow window;
        //ofSetupOpenGL(&window, 600,300, OF_WINDOW);
        
        //ofSetupOpenGL(600,300, OF_WINDOW);			// <-------- setup the GL context
        // can be OF_WINDOW or OF_FULLSCREEN
        //ofSetFrameRate(50);
        
       
        //ofGetMainLoop()->pollEvents = 0; // YOU NEED TO MAKE "void (*pollEvents)(void)" PUBLIC !!!
        
        ofRunApp(new ofApp());
        //while(1) usleep(10000);
    }
    
} ;//thread;

//--------------------------------------------------------------
void ofApp::exit(){
    
}

//--------------------------------------------------------------
void ofApp::setup(){
    //ofRegisterTouchEvents(this);
    ofSetEscapeQuitsApp(false);

    pointerImage.allocate(56, 80, OF_IMAGE_COLOR_ALPHA);
    int i = 0;
    while ( i < pointerImage.getPixels().size() ) {
        pointerImage.getPixels()[i] = pointer56.pixel_data[i];
        i++;
    }
    pointerImage.update(); //reloadTexture();
}

//--------------------------------------------------------------
void ofApp::update(){
    pofBase::updateAll();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255, 255);
    pofBase::drawAll();
    ofSetupScreen();
    ofSetColor(255, 255, 255);
    //pointerImage.draw(ofGetMouseX()-3, ofGetMouseY()-4, 0, 17, 24);
    /*ofCircle(ofGetMouseX(), ofGetMouseY(), 2);
    ofSetColor(0, 0, 0);
    ofCircle(ofGetMouseX(), ofGetMouseY(), 1);*/
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
    //cerr << "mouseDragged ..."<<endl;
    pofBase::touchMovedAll(x, y, 0);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //cerr << "mousePressed ..."<<endl;
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

void pollEventsMethod(void* nul)
{
    glfwPollEvents(); // REMOVE THE ONE IN ofAppGLFWWindow::display() !!

    clock_delay(pollEventsClock,2); //poll events every 2ms
}

void open_window(int argc, t_atom *argv)
{
    static bool opened = false;
    if(opened) return;
    opened = true;
    while(argc)
    {
        if(argv->a_type == A_SYMBOL && atom_getsymbol(argv) == gensym("noborder")) {
            windowNoBorder = true;
            post("no border");
        }
        argc--; argv++;
    }

//    ofSetupOpenGL(600,300, OF_WINDOW);
    ofGLFWWindowSettings settings;
    if(windowNoBorder) settings.decorated = false;
    settings.width= 800;
    settings.height= 600;
    ofCreateWindow(settings);

    (new MyThread)->startThread();

    pollEventsClock = clock_new(0,(t_method)pollEventsMethod);
    clock_delay(pollEventsClock,100);
}

extern "C" {
    /* this is called once at setup time, when this code is loaded into Pd. */
    void pof_setup(void)
    {
        pofBase::setup();

        pofWin::open = &open_window;
/*        ofSetupOpenGL(600,300, OF_WINDOW);
        (new MyThread)->startThread();

        pollEventsClock = clock_new(0,(t_method)pollEventsMethod);
        clock_delay(pollEventsClock,100);*/
    }
}


