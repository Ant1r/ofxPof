/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofRect.h"

t_class *pofrect_class;

#define NEXT_FLOAT_ARG(var) if(argc>0) { if(argv->a_type == A_FLOAT) var = atom_getfloat(argv); argv++; argc--; }

void *pofrect_new(t_symbol *s, int argc, t_atom *argv)
{
    float w=0, h=0, tlR=0, trR=0, brR=0, blR=0;
    int res = 0;

    NEXT_FLOAT_ARG(w);
    NEXT_FLOAT_ARG(h);
    NEXT_FLOAT_ARG(tlR);
    NEXT_FLOAT_ARG(trR);
    NEXT_FLOAT_ARG(brR);
    NEXT_FLOAT_ARG(blR);
    NEXT_FLOAT_ARG(res);
    
    pofRect* obj = new pofRect(pofrect_class, w, h, tlR, trR, brR, blR, res);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    
    return (void*) (obj->pdobj);
}

void pofrect_free(void *x)
{
	delete (pofRect*)(((PdObject*)x)->parent);
}

void pofrect_rounded(void *x, t_floatarg tlR, t_floatarg trR, t_floatarg brR, t_floatarg blR)
{
	pofRect* px = (pofRect*)(((PdObject*)x)->parent);
	
	px->topLeftR = tlR;
	px->topRightR = trR;
	px->bottomRightR = brR;
	px->bottomLeftR = blR;
}

void pofrect_res(void *x, float res)
{
	pofRect* px = (pofRect*)(((PdObject*)x)->parent);
	px->resolution = int(res);
}

void pofRect::setup(void)
{
	//post("pofrect_setup");
	pofrect_class = class_new(gensym("pofrect"), (t_newmethod)pofrect_new, (t_method)pofrect_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofrect_class, (t_method)pofrect_rounded, gensym("rounded"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofrect_class, (t_method)pofrect_res, gensym("res"), A_FLOAT, A_NULL);

	POF_SETUP(pofrect_class);
}

void pofRect::draw()
{
	float h = height;
	if(h==0) h = width;

	if(resolution) ofSetCircleResolution(resolution);
	
	if(topRightR==0 && bottomRightR==0 && bottomLeftR==0) {
		if(topLeftR==0) ofRect(-width/2, -h/2, width, h);
		else ofRectRounded(-width/2, -h/2, width, h, topLeftR);
	} else ofRectRounded(-width/2, -h/2, 0, width, h, topLeftR, topRightR, bottomRightR, bottomLeftR);
}

