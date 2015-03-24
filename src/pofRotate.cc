/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofRotate.h"

t_class *pofRotate_class;

void *pofRotate_new(t_symbol *s, int argc, t_atom *argv)
{
    float a = 0;
    tAxis axis = Z;
    t_symbol *saxis=0;

	while(argc) {
		if(argv->a_type == A_FLOAT) a = atom_getfloat(argv);
		else if(argv->a_type == A_SYMBOL) saxis = atom_getsymbol(argv);
		argv++;
		argc--;
	}
	
	if(saxis && saxis->s_name && *saxis->s_name) {
		if(saxis == gensym("x")) axis = X;
		else if(saxis == gensym("y")) axis = Y;
	}
	
    pofRotate* obj = new pofRotate(pofRotate_class, a, axis);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->a);
    
    return (void*) (obj->pdobj);
}

void pofRotate_free(void *x)
{
	delete (pofRotate*)(((PdObject*)x)->parent);
}


void pofRotate::setup(void)
{
	//post("pofrotate_setup");
	pofRotate_class = class_new(gensym("pofrotate"), (t_newmethod)pofRotate_new, (t_method)pofRotate_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofRotate_class);
}

void pofRotate::draw()
{
	aReal = a;

	if(axis == X) axisV.set(1,0,0);
	else if(axis == Y) axisV.set(0,1,0);
	else axisV.set(0,0,1);

	ofRotate(aReal, axisV.x, axisV.y, axisV.z);
}

void pofRotate::postdraw()
{
	ofRotate(-aReal, axisV.x, axisV.y, axisV.z);
}

bool pofRotate::computeTouch(int &xx, int &yy)
{
	if(axis != Z) return false;
	
	ofVec2f v = ofVec2f(xx,yy);
	v.rotate(-a);
	xx = v.x;
	yy = v.y;
	
	return true;
}
