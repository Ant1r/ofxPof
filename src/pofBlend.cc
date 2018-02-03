/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBlend.h"

t_class *pofBlend_class;

void pofBlend_float(void *x, t_float t);

void *pofBlend_new(t_floatarg visible)
{
    pofBlend* obj = new pofBlend(pofBlend_class,visible);
    
    return (void*) (obj->pdobj);
}

void pofBlend_free(void *x)
{
	delete (pofBlend*)(((PdObject*)x)->parent);
}

void pofBlend::setup(void)
{
	pofBlend_class = class_new(gensym("pofblend"), (t_newmethod)pofBlend_new, (t_method)pofBlend_free,
		sizeof(PdObject), 0, A_NULL);
	POF_SETUP(pofBlend_class);
	//class_addfloat(pofBlend_class, (t_method)pofBlend_float);
}

void pofBlend::draw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void pofBlend::postdraw()
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
