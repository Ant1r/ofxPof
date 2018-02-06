/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFill.h"

float pofFill::currentLineWidth = 1.0;

t_class *pofFill_class;

static void *pofFill_new(t_symbol *src, t_float fill, t_float linewidth)
{
	pofFill* obj = new pofFill(pofFill_class);
	
	obj->fill = (ofFillFlag)fill;		
	obj->lineWidth = (ofFillFlag)linewidth;		
	
	return (void*) (obj->pdobj);
}

static void pofFill_free(void *x)
{
	delete (pofFill*)(((PdObject*)x)->parent);
}

static void pofFill_float(void *x, t_float fill)
{
	pofFill* px = (pofFill*)(((PdObject*)x)->parent);
	px->fill = (ofFillFlag)fill;
}

static void pofFill_linewidth(void *x, t_float linewidth)
{
	pofFill* px = (pofFill*)(((PdObject*)x)->parent);
	px->lineWidth = linewidth;
}

void pofFill::setup(void)
{
	pofFill_class = class_new(gensym("poffill"), (t_newmethod)pofFill_new, (t_method)pofFill_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofFill_class);
	class_addfloat(pofFill_class, (t_method)pofFill_float);
	class_addmethod(pofFill_class, (t_method)pofFill_linewidth, gensym("linewidth"), A_FLOAT, A_NULL);
}

void pofFill::draw()
{
	lastFill = ofGetFill();
	if(fill) ofFill();
	else ofNoFill();
	lastLineWidth = currentLineWidth;
	if(lineWidth) {
		currentLineWidth = lineWidth;
		ofSetLineWidth(currentLineWidth);
	}   
}

void pofFill::postdraw()
{
    if(lastFill) ofFill();
    else ofNoFill();
	currentLineWidth = lastLineWidth;
	ofSetLineWidth(currentLineWidth);
}
