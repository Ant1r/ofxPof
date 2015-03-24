#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main(){
	ofSetupOpenGL(400, 400, OF_WINDOW);
	ofSetFrameRate(50);
	ofRunApp(new testApp());
}
