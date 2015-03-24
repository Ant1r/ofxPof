/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTouchable.h"

t_class *poftouchable_class;

void *poftouchable_new(t_floatarg touchable)
{
    pofTouchable* obj = new pofTouchable(poftouchable_class,touchable);
    
    return (void*) (obj->pdobj);
}

void poftouchable_free(void *x)
{
	delete (pofTouchable*)(((PdObject*)x)->parent);
}

void poftouchable_float(void *x, t_float t)
{
	((pofTouchable*)(((PdObject*)x)->parent))->touchable = (t!=0);
}

void pofTouchable::setup(void)
{
	//post("poftouchable_setup");
	poftouchable_class = class_new(gensym("poftouchable"), (t_newmethod)poftouchable_new,
		(t_method)poftouchable_free,sizeof(PdObject), 0, A_DEFFLOAT, A_NULL);
	POF_SETUP(poftouchable_class);
	class_addfloat(poftouchable_class, (t_method)poftouchable_float);	
}

void pofTouchable::update()
{
	if(realTouchable!=touchable) {
		if(!touchable) pofBase::tree_touchCancel();
		realTouchable=touchable;
	}
}

bool pofTouchable::tree_touchMoved(int x, int y, int id)
{
	if(realTouchable) return pofBase::tree_touchMoved(x, y, id);
	else return false;
}

bool pofTouchable::tree_touchDown(int x, int y, int id)
{
	if(realTouchable) return pofBase::tree_touchDown(x, y, id);
	else return false;
}

bool pofTouchable::tree_touchUp(int x, int y, int id)
{
	if(realTouchable) return pofBase::tree_touchUp(x, y, id);
	else return false;
}

bool pofTouchable::tree_touchDoubleTap(int x, int y, int id)
{
	if(realTouchable) return pofBase::tree_touchDoubleTap(x, y, id);
	else return false;
}

void pofTouchable::tree_touchCancel()
{
	if(realTouchable) pofBase::tree_touchCancel();
}

