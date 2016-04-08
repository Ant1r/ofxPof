#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main(int argc, char *argv[]){
	ofSetupOpenGL(400, 400, OF_WINDOW);
	ofSetFrameRate(50);
	
	testApp *app = new testApp();
	app->arguments = vector<string>(argv, argv + argc);
	ofRunApp(app);
}
