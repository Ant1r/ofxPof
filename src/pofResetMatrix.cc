/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofResetMatrix.h"

t_class *pofResetMatrix_class;

void *pofResetMatrix_new()
{
    pofResetMatrix* obj = new pofResetMatrix(pofResetMatrix_class);
    return (void*) (obj->pdobj);
}

void pofResetMatrix_free(void *x)
{
	delete (pofResetMatrix*)(((PdObject*)x)->parent);
}

static void pofResetMatrix_matrix(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofResetMatrix* px= (pofResetMatrix*)(((PdObject*)x)->parent);

	for(int i = 0 ; i < 16 ; i++) {
		if(argc && (argv->a_type == A_FLOAT)) {
			px->matrix(i%4, i/4) = atom_getfloat(argv); 
			argc-- ; argv++;
		} else {
			px->hasMatrix = false;
			return;
		}
	}
	px->hasMatrix = true;
}

void pofResetMatrix::setup(void)
{
	//post("pofresetmatrix_setup");
	pofResetMatrix_class = class_new(gensym("pofresetmatrix"), (t_newmethod)pofResetMatrix_new, (t_method)pofResetMatrix_free,
		sizeof(PdObject), 0, A_NULL);
	class_addmethod(pofResetMatrix_class, (t_method)pofResetMatrix_matrix, gensym("matrix"), A_GIMME, A_NULL);
	POF_SETUP(pofResetMatrix_class);
}

void pofResetMatrix::draw()
{
	ofPushMatrix();
	ofSetupScreen();
	ofTranslate(ofGetWindowWidth()/2,ofGetWindowHeight()/2);
	if(hasMatrix) ofLoadMatrix(matrix);
}

void pofResetMatrix::postdraw()
{
	ofPopMatrix();
}


