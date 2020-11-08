/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBlend.h"

GLenum pofBlend::currentSrcFactor = GL_SRC_ALPHA;
GLenum pofBlend::currentDestFactor = GL_ONE_MINUS_SRC_ALPHA;

t_class *pofBlend_class;

static GLenum getFactor(t_symbol *factor)
{
	if(factor == gensym("ONE")) return GL_ONE;
	if(factor == gensym("SRC_COLOR")) return GL_SRC_COLOR;
	if(factor == gensym("ONE_MINUS_SRC_COLOR")) return GL_ONE_MINUS_SRC_COLOR;
	if(factor == gensym("SRC_ALPHA")) return GL_SRC_ALPHA;
	if(factor == gensym("ONE_MINUS_SRC_ALPHA")) return GL_ONE_MINUS_SRC_ALPHA;
	if(factor == gensym("DST_ALPHA")) return GL_DST_ALPHA;
	if(factor == gensym("ONE_MINUS_DST_ALPHA")) return GL_ONE_MINUS_DST_ALPHA;
	if(factor == gensym("DST_COLOR")) return GL_DST_COLOR;
	if(factor == gensym("ONE_MINUS_DST_COLOR")) return GL_ONE_MINUS_DST_COLOR;
	if(factor == gensym("SRC_ALPHA_SATURATE")) return GL_SRC_ALPHA_SATURATE;
	return GL_ZERO;
}

void *pofBlend_new(t_symbol *src, t_symbol *dest)
{
	pofBlend* obj = new pofBlend(pofBlend_class);

	if(src != &s_) obj->srcFactor = getFactor(src);
	if(dest!= &s_) obj->destFactor = getFactor(dest);
	return (void*) (obj->pdobj);
}

void pofBlend_free(void *x)
{
	delete (pofBlend*)(((PdObject*)x)->parent);
}

static void pofBlend_set(void *x, t_symbol *src, t_symbol *dest)
{
	pofBlend* px = (pofBlend*)(((PdObject*)x)->parent);
	px->srcFactor = getFactor(src);
	px->destFactor = getFactor(dest);
}

void pofBlend::setup(void)
{
	pofBlend_class = class_new(gensym("pofblend"), (t_newmethod)pofBlend_new, (t_method)pofBlend_free,
		sizeof(PdObject), 0, A_DEFSYM, A_DEFSYM, A_NULL);
	POF_SETUP(pofBlend_class);
	class_addmethod(pofBlend_class, (t_method)pofBlend_set, gensym("set"), A_DEFSYM, A_DEFSYM, A_NULL);
}

void pofBlend::draw()
{
	lastSrcFactor = currentSrcFactor;
	lastDestFactor = currentDestFactor;
	currentSrcFactor = srcFactor;
	currentDestFactor = destFactor;
	glBlendFunc(currentSrcFactor, currentDestFactor);
}

void pofBlend::postdraw()
{
	currentSrcFactor = lastSrcFactor;
	currentDestFactor = lastDestFactor;
	glBlendFunc(currentSrcFactor, currentDestFactor);
}
