/*
 * Copyright (c) 2011 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxPd for documentation
 *
 */
#include "ofMain.h"
#include "ofApp.h"

//========================================================================

#if OF_VERSION_MAJOR == 0 && OF_VERSION_MINOR <= 8
extern "C" {
    size_t fwrite$UNIX2003(const void *a, size_t b, size_t c, FILE *d) {
        return fwrite(a, b, c, d);
    }
    char* strerror$UNIX2003(int errnum) {
        return strerror(errnum);
    }
    time_t mktime$UNIX2003(struct tm * a) {
        return mktime(a);
    }
    double strtod$UNIX2003(const char * a, char ** b) {
        return strtod(a, b);
    }
}
#endif


int main(){
	ofSetupOpenGL(2000, 1500, OF_FULLSCREEN);
	ofRunApp(new ofApp());
}

