/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofSphere.h"

t_class *pofsphere_class;

void *pofsphere_new(t_floatarg r, t_floatarg res)
{
    pofSphere* obj = new pofSphere(pofsphere_class, r, res);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->radius);
    return (void*) (obj->pdobj);
}

void pofsphere_free(void *x)
{
	delete (pofSphere*)(((PdObject*)x)->parent);
}

void pofsphere_res(void *x, float res)
{
	pofSphere* px = (pofSphere*)(((PdObject*)x)->parent);
	
	px->resolution = int(res);
}

void pofSphere::setup(void)
{
	//post("pofsphere_setup");
	pofsphere_class = class_new(gensym("pofsphere"), (t_newmethod)pofsphere_new, (t_method)pofsphere_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofsphere_class, (t_method)pofsphere_res, gensym("res"), A_FLOAT, A_NULL);
	POF_SETUP(pofsphere_class);
}

void pofSphere::draw()
{
	//ofEnableNormalizedTexCoords();
	if(resolution) ofSetSphereResolution(resolution);

    ofDrawSphere(radius);
    //ofDisableNormalizedTexCoords();
}
