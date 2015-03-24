/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTranslate.h"

t_class *pofTranslate_class;

void *pofTranslate_new(t_floatarg x, t_floatarg y)
{
    pofTranslate* obj = new pofTranslate(pofTranslate_class, x, y);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.x);
    floatinlet_new(&obj->pdobj->x_obj, &obj->v.y);
    return (void*) (obj->pdobj);
}

void pofTranslate_free(void *x)
{
	delete (pofTranslate*)(((PdObject*)x)->parent);
}


void pofTranslate::setup(void)
{
	//post("poftranslate_setup");
	pofTranslate_class = class_new(gensym("poftranslate"), (t_newmethod)pofTranslate_new, (t_method)pofTranslate_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofTranslate_class);
}

void pofTranslate::draw()
{
	vReal = v;
	ofTranslate(vReal.x, vReal.y);
}

void pofTranslate::postdraw()
{
	ofTranslate(-vReal.x, -vReal.y);
}

bool pofTranslate::computeTouch(int &xx, int &yy)
{
	xx -= v.x;
	yy -= v.y;
	return true;
}
