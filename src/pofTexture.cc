/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofTexture.h"

t_class *poftexture_class;

void *poftexture_new(t_symbol *name)
{
    float w = 0, h = 0;
    
    pofTexture* obj = new pofTexture(poftexture_class, name);
    
    return (void*) (obj->pdobj);
}

void poftexture_free(void *x)
{
	pofTexture *px = (pofTexture*)(((PdObject*)x)->parent);
	delete px;
}

void poftexture_set(void *x, t_symbol *name)
{
	pofTexture *px = (pofTexture*)(((PdObject*)x)->parent);
	
	px->name = name;
}

void pofTexture::setup(void)
{
	//post("poftexture_setup");
	poftexture_class = class_new(gensym("poftexture"), (t_newmethod)poftexture_new, (t_method)poftexture_free,
		sizeof(PdObject), 0, A_DEFSYMBOL, A_NULL);
	class_addmethod(poftexture_class, (t_method)poftexture_set, gensym("set"), A_SYMBOL, A_NULL);
	POF_SETUP(poftexture_class);
}

void pofTexture::draw()
{
	if(!name) return;
	
	ofTexture *texture = pofBase::textures[name];
	if(texture) {
		texture->bind();
		pofBase::currentTexture = texture;
	}
}

void pofTexture::postdraw()
{
	if(!name) return;
	
	ofTexture *texture = pofBase::textures[name];
	if(texture) texture->unbind();
}

