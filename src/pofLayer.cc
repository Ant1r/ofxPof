/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofLayer.h"

t_symbol *pofLayer::currentLayer = NULL;
static t_symbol *s_none = NULL;
t_class *pofLayer_class;

static void *pofLayer_new(t_symbol *_layer)
{
	pofLayer* obj = new pofLayer(pofLayer_class);
	
	obj->layer = _layer;
	
	return (void*) (obj->pdobj);
}

static void pofLayer_free(void *x)
{
	delete (pofLayer*)(((PdObject*)x)->parent);
}

static void pofLayer_layer(void *x, t_symbol *newlayer)
{
	pofLayer* px = (pofLayer*)(((PdObject*)x)->parent);
	px->layer = newlayer;
}

void pofLayer::setup(void)
{
	s_none = gensym("");
	
	pofLayer_class = class_new(gensym("poflayer"), (t_newmethod)pofLayer_new, (t_method)pofLayer_free,
		sizeof(PdObject), 0, A_DEFSYM, A_NULL);
	POF_SETUP(pofLayer_class);
	class_addmethod(pofLayer_class, (t_method)pofLayer_layer, gensym("layer"), A_DEFSYM, A_NULL);

	ofAddListener(pofBase::initFrameEvent, &pofLayer::initFrame);
}

void pofLayer::draw()
{
	lastLayer = currentLayer;
	if(layer) currentLayer = layer;
}

void pofLayer::postdraw()
{
	currentLayer = lastLayer;
}

void pofLayer::initFrame(ofEventArgs & args)
{
	currentLayer = s_none;
}
