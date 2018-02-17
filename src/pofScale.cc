/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofScale.h"

t_class *pofScale_class;

void *pofScale_new(/*t_floatarg x,t_floatarg y, t_floatarg z*/t_symbol *s, int argc, t_atom *argv)
{
    pofScale* obj /*= new pofScale(pofScale_class, x, y, z)*/;
    float f, x, y, z;
    int n = 0;
    
    x = y = z = 1;
    
    while(argc) {
		if(argv->a_type == A_FLOAT) {
			n++;
			f = atom_getfloat(argv);
			switch(n) {
				case 1 : x = f; break;
				case 2 : y = f; break;
				case 3 : z = f; break;
			}
		}	
		else break;
		argv++;
		argc--;
	}
     
    obj = new pofScale(pofScale_class, x, y, z);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.x);
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.y);
    if (n >= 3) floatinlet_new(&obj->pdobj->x_obj, &obj->v.z);
    
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
		sizeof(PdObject), 0, A_GIMME/*A_DEFFLOAT, A_DEFFLOAT*/, A_NULL);
	POF_SETUP(pofScale_class);
}

void pofScale::draw()
{
	if(v.x && v.y && v.z) vReal = v;
	else vReal = ofVec3f(1.0, 1.0, 1.0);
	
	ofScale(vReal.x, vReal.y, vReal.z);
}

void pofScale::postdraw()
{
	ofScale(1.0/vReal.x, 1.0/vReal.y, 1.0/vReal.z);
}

bool pofScale::computeTouch(int &xx, int &yy)
{
	if(v.x && v.y && v.z) {
		xx /= v.x;
		yy /= v.y;
	}
	return true;
}
