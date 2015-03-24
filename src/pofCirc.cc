/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofCirc.h"

t_class *pofcirc_class;

void *pofcirc_new(t_floatarg w,t_floatarg h, t_floatarg res)
{
    pofCirc* obj = new pofCirc(pofcirc_class, w, h, res);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    return (void*) (obj->pdobj);
}

void pofcirc_free(void *x)
{
	delete (pofCirc*)(((PdObject*)x)->parent);
}

void pofcirc_res(void *x, float res)
{
	pofCirc* px = (pofCirc*)(((PdObject*)x)->parent);
	
	px->resolution = int(res);
}

void pofCirc::setup(void)
{
	//post("pofcirc_setup");
	pofcirc_class = class_new(gensym("pofcirc"), (t_newmethod)pofcirc_new, (t_method)pofcirc_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofcirc_class, (t_method)pofcirc_res, gensym("res"), A_FLOAT, A_NULL);
	POF_SETUP(pofcirc_class);
}

void pofCirc::draw()
{
	if(resolution) ofSetCircleResolution(resolution);

    if(height == 0) ofEllipse(0, 0, width, width);
	else ofEllipse(0, 0, width, height);
}
