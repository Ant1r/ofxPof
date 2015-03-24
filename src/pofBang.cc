/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBang.h"

t_class *pofbang_class;

void *pofbang_new(t_floatarg _enable)
{    
    pofBang* obj = new pofBang(pofbang_class, _enable);
    
    return (void*) (obj->pdobj);
}

void pofbang_free(void *x)
{
	delete (pofBang*)(((PdObject*)x)->parent);
}

static void pofbang_bang(void *x)
{
    pofBang* px = (pofBang*)(((PdObject*)x)->parent);    
    outlet_bang(px->m_out2);
}

static void pofbang_float(void *x, t_floatarg f)
{
    pofBang* px = (pofBang*)(((PdObject*)x)->parent);    
    px->enable = (f != 0);
}

void pofBang::draw()
{
	if(enable) {
		t_atom at;
		SETSYMBOL(&at, &s_bang);
		queueToSelfPd(1, &at);
	}
}

void pofBang::setup(void)
{
	//post("pofbang_setup");
	pofbang_class = class_new(gensym("pofbang"), (t_newmethod)pofbang_new,
		(t_method)pofbang_free,sizeof(PdObject), 0, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofbang_class);
	class_addfloat(pofbang_class, (t_method)pofbang_float);	
	class_addbang(pofbang_class, (t_method)pofbang_bang);	
}

