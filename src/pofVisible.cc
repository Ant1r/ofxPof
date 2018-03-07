/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofVisible.h"
#include "pofLayer.h"

t_class *pofvisible_class;

void pofvisible_float(void *x, t_float t);

void *pofvisible_new(t_symbol *sym,int argc, t_atom *argv)
{
	float visible = 0;
	t_symbol *layer = NULL;
 
	pofVisible* obj = new pofVisible(pofvisible_class,visible);

	if(argc && argv->a_type == A_SYMBOL) {
		layer = atom_getsymbol(argv); argc--; argv++;
		visible = 1; // visible defaults to 1 when layer is given		
		if(argc && argv->a_type == A_FLOAT) {
			visible = atom_getfloat(argv);
		}
	} else if(argc && argv->a_type == A_FLOAT) {
		visible = atom_getfloat(argv); argc--; argv++;
		if(argc && argv->a_type == A_SYMBOL) layer = atom_getsymbol(argv);
	}

	pofvisible_float((void*) (obj->pdobj), visible);
	obj->layer = layer;
	return (void*) (obj->pdobj);
}

void pofvisible_free(void *x)
{
	delete (pofVisible*)(((PdObject*)x)->parent);
}

void pofvisible_float(void *x, t_float t)
{
	pofVisible *px = (pofVisible*)(((PdObject*)x)->parent);
	
	px->touchable = (t!=0);
	px->visible = px->touchable;
}


void pofVisible::setup(void)
{
	//post("pofvisible_setup");
	pofvisible_class = class_new(gensym("pofvisible"), (t_newmethod)pofvisible_new, (t_method)pofvisible_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofvisible_class);
	class_addfloat(pofvisible_class, (t_method)pofvisible_float);
}

void pofVisible::tree_draw()
{
	if( visible && ((layer == NULL) || (layer == pofLayer::currentLayer)) )
		pofBase::tree_draw();
}

