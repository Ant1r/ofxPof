/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofScale.h"

t_class *pofScale_class;

void *pofScale_new(t_floatarg x,t_floatarg y)
{
    pofScale* obj = new pofScale(pofScale_class, x, y);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.x);
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.y);
    return (void*) (obj->pdobj);
}

void pofScale_free(void *x)
{
	delete (pofScale*)(((PdObject*)x)->parent);
}


void pofScale::setup(void)
{
	//post("pofscale_setup");
	pofScale_class = class_new(gensym("pofscale"), (t_newmethod)pofScale_new, (t_method)pofScale_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofScale_class);
}

void pofScale::draw()
{
	vReal = v;
	ofScale(vReal.x, vReal.y);
}

void pofScale::postdraw()
{
	ofScale(1.0/vReal.x,1.0/vReal.y);
}

bool pofScale::computeTouch(int &xx, int &yy)
{
	xx /= v.x;
	yy /= v.y;
	return true;
}
