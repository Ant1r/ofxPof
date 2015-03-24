/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofAccel.h"
#include "ofxAccelerometer.h"

t_class *pofaccel_class;

void *pofaccel_new()
{
    pofAccel* obj = new pofAccel(pofaccel_class);

    return (void*) (obj->pdobj);
}

void pofaccel_free(void *x)
{
	delete (pofAccel*)(((PdObject*)x)->parent);
}

void pofaccel_getforce(void *x)
{
	pofAccel* px= (pofAccel*)(((PdObject*)x)->parent);
	
	t_atom ap[3];
	
	SETFLOAT(&ap[0], ofxAccelerometer.getForce().x);
	SETFLOAT(&ap[1], ofxAccelerometer.getForce().y);
	SETFLOAT(&ap[2], ofxAccelerometer.getForce().z);
	outlet_anything(px->m_out1, gensym("force"), 3, ap);
}

void pofaccel_getorient(void *x)
{
	pofAccel* px= (pofAccel*)(((PdObject*)x)->parent);
	
	t_atom ap[2];
	
	SETFLOAT(&ap[0], ofxAccelerometer.getOrientation().x);
	SETFLOAT(&ap[1], ofxAccelerometer.getOrientation().y);
	outlet_anything(px->m_out1, gensym("orient"), 2, ap);
}

void pofAccel::setup(void)
{
	//post("pofaccel_setup");
	pofaccel_class = class_new(gensym("pofaccel"), (t_newmethod)pofaccel_new, (t_method)pofaccel_free,
		sizeof(PdObject), 0, A_NULL);
	//POF_SETUP(pofaccel_class);
	class_addmethod(pofaccel_class, (t_method)pofaccel_getforce, gensym("getforce"), A_NULL);
	class_addmethod(pofaccel_class, (t_method)pofaccel_getorient, gensym("getorient"), A_NULL);
}


