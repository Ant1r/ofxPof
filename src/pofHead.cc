/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofHead.h"

std::list<pofHead*> pofHead::pofheads;

t_class *pofhead_class;

void *pofhead_new(void)
{
    pofHead* obj = new pofHead(pofhead_class);
    return (void*) (obj->pdobj);
}

void pofhead_free(void *x)
{
	delete (pofHead*)(((PdObject*)x)->parent);
}

void pofHead::draw(void)
{
	//ofLoadIdentityMatrix();
	ofSetupScreen();
	ofColor(255,255,255,255);
	ofTranslate(ofGetWindowWidth()/2,ofGetWindowHeight()/2);
}

void pofHead::setup(void)
{
	//post("pofhead_setup");
	pofhead_class = class_new(gensym("pofhead"), (t_newmethod)pofhead_new, (t_method)pofhead_free,
		sizeof(PdObject), 0, (t_atomtype)0);
	POF_SETUP(pofhead_class);
}

