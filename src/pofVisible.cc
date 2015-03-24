/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofVisible.h"

t_class *pofvisible_class;

void pofvisible_float(void *x, t_float t);

void *pofvisible_new(t_floatarg visible)
{
    pofVisible* obj = new pofVisible(pofvisible_class,visible);
    
    pofvisible_float((void*) (obj->pdobj), visible);
    
    return (void*) (obj->pdobj);
}

void pofvisible_free(void *x)
{
	delete (pofVisible*)(((PdObject*)x)->parent);
}

void pofvisible_float(void *x, t_float t)
{
	pofVisible *px = (pofVisible*)(((PdObject*)x)->parent);
	
	px->touchable = (t!=0);
	px->visible = px->touchable;
}


void pofVisible::setup(void)
{
	//post("pofvisible_setup");
	pofvisible_class = class_new(gensym("pofvisible"), (t_newmethod)pofvisible_new, (t_method)pofvisible_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofvisible_class);
	class_addfloat(pofvisible_class, (t_method)pofvisible_float);	
}

void pofVisible::tree_draw()
{
	if(visible) pofBase::tree_draw();
}

