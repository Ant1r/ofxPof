#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main(){
	ofSetupOpenGL(400, 400, OF_WINDOW);
	ofSetFrameRate(50);
	ofRunApp(new testApp());
}
