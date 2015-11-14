/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofPlane.h"

t_class *pofplane_class;

#define NEXT_FLOAT_ARG(var) if(argc>0) { if(argv->a_type == A_FLOAT) var = atom_getfloat(argv); argv++; argc--; }

void *pofplane_new(t_symbol *s, int argc, t_atom *argv)
{
    float w=0, h=0;

    NEXT_FLOAT_ARG(w);
    NEXT_FLOAT_ARG(h);
    
    pofPlane* obj = new pofPlane(pofplane_class, w, h);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    
    return (void*) (obj->pdobj);
}

void pofplane_free(void *x)
{
	delete (pofPlane*)(((PdObject*)x)->parent);
}

void pofplane_res(void *x, float resX, float resY)
{
	pofPlane* px = (pofPlane*)(((PdObject*)x)->parent);
	ofSetPlaneResolution(resX, resY);
}

void pofPlane::setup(void)
{
	//post("pofplane_setup");
	pofplane_class = class_new(gensym("pofplane"), (t_newmethod)pofplane_new, (t_method)pofplane_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofplane_class, (t_method)pofplane_res, gensym("res"), A_FLOAT, A_FLOAT, A_NULL );

	POF_SETUP(pofplane_class);
}

void pofPlane::draw()
{
	float h = height;
	if(h==0) h = width;

	ofDrawPlane(width, h);
}

