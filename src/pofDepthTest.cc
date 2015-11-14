/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofDepthTest.h"

t_class *pofDepthTest_class;

void *pofDepthTest_new()
{
    pofDepthTest* obj = new pofDepthTest(pofDepthTest_class);
    
    return (void*) (obj->pdobj);
}

void pofDepthTest_free(void *x)
{
	delete (pofDepthTest*)(((PdObject*)x)->parent);
}

void pofDepthTest_float(void *x, t_float t)
{
	pofDepthTest *px = (pofDepthTest*)(((PdObject*)x)->parent);
	
	px->enable = (t!=0);
}

void pofDepthTest::setup(void)
{
	//post("pofDepthTest_setup");
	pofDepthTest_class = class_new(gensym("pofdepthtest"), (t_newmethod)pofDepthTest_new, (t_method)pofDepthTest_free,
		sizeof(PdObject), 0, A_NULL);
	class_addfloat(pofDepthTest_class, (t_method)pofDepthTest_float);
	POF_SETUP(pofDepthTest_class);
}

void pofDepthTest::draw()
{
	if(enable) ofEnableDepthTest();
}

void pofDepthTest::postdraw()
{
	ofDisableDepthTest();
}
