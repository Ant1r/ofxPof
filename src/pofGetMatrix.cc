/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofGetMatrix.h"

t_class *pofGetMatrix_class;
static t_symbol* s_dosend;

void *pofGetMatrix_new()
{
    pofGetMatrix* obj = new pofGetMatrix(pofGetMatrix_class);
    return (void*) (obj->pdobj);
}

void pofGetMatrix_free(void *x)
{
	delete (pofGetMatrix*)(((PdObject*)x)->parent);
}

static void pofGetMatrix_bang(void *x)
{
	pofGetMatrix* px= (pofGetMatrix*)(((PdObject*)x)->parent);
	px->trigger = true;
}

static void pofGetMatrix_dosend(void *x)
{
	pofGetMatrix* px= (pofGetMatrix*)(((PdObject*)x)->parent);
	t_atom at[16];

	for(int i = 0 ; i < 16 ; i++) SETFLOAT(&at[i],px->matrix(i%4, i/4));
	outlet_anything(px->m_out2, gensym("list"), 16, at);

	ofVec3f trans = px->matrix.getTranslation();
	SETFLOAT(&at[0],trans.x);
	SETFLOAT(&at[1],trans.y);
	SETFLOAT(&at[2],trans.z);
	outlet_anything(px->m_out3, gensym("translate"), 3, at);

	float angle;
	ofVec3f rot;
	px->matrix.getRotate().getRotate(angle, rot);
	SETFLOAT(&at[0],angle);
	SETFLOAT(&at[1],rot.x);
	SETFLOAT(&at[2],rot.y);
	SETFLOAT(&at[3],rot.z);
	outlet_anything(px->m_out3, gensym("rotate"), 4, at);
	
	ofVec3f scale = px->matrix.getScale();
	SETFLOAT(&at[0],scale.x);
	SETFLOAT(&at[1],scale.y);
	SETFLOAT(&at[2],scale.z);
	outlet_anything(px->m_out3, gensym("scale"), 3, at);
}

void pofGetMatrix::setup(void)
{
	//post("pofresetmatrix_setup");
	s_dosend = gensym("dosend");
	pofGetMatrix_class = class_new(gensym("pofgetmatrix"), (t_newmethod)pofGetMatrix_new, (t_method)pofGetMatrix_free,
		sizeof(PdObject), 0, A_NULL);
	class_addbang(pofGetMatrix_class, (t_method)pofGetMatrix_bang);
	class_addmethod(pofGetMatrix_class, (t_method)pofGetMatrix_dosend, s_dosend, A_NULL);
	POF_SETUP(pofGetMatrix_class);
}

void pofGetMatrix::draw()
{
	if(trigger) {
		t_atom at;
		SETSYMBOL(&at, s_dosend);
		trigger = false;
		matrix = ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
		queueToSelfPd(1, &at);	
	}
}


