/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofColor.h"

t_class *pofColor_class;

void *pofColor_new(t_floatarg r,t_floatarg g,t_floatarg b,t_floatarg a)
{
    pofColor* obj = new pofColor(pofColor_class, r,g,b,a);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->r);
    floatinlet_new(&obj->pdobj->x_obj, &obj->g);
    floatinlet_new(&obj->pdobj->x_obj, &obj->b);
    floatinlet_new(&obj->pdobj->x_obj, &obj->a);
    return (void*) (obj->pdobj);
}

void pofColor_free(void *x)
{
	delete (pofColor*)(((PdObject*)x)->parent);
}


void pofColor::setup(void)
{
	//post("pofColor_setup");
	pofColor_class = class_new(gensym("pofcolor"), (t_newmethod)pofColor_new, (t_method)pofColor_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT,A_NULL);
	POF_SETUP(pofColor_class);
}

void pofColor::draw()
{
	styleColor = ofGetStyle().color;
	ofSetColor(styleColor.r*r,styleColor.g*g,styleColor.b*b,styleColor.a*a);
}

void pofColor::postdraw()
{
	ofSetColor(styleColor);
}
