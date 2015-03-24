/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFont.h"

t_class *poffont_class;

std::map<t_symbol*,pofFont*> pofFont::fonts;

void *poffont_new(t_symbol *font, t_symbol *fontfile, t_float size)
{
	if(pofFont::fonts.find(font)!=pofFont::fonts.end()) {
    	post("poffont error : duplicate font name %s",font->s_name);
    	return NULL;
    }
    
    pofFont* obj = new pofFont(poffont_class, font, fontfile, size);
    obj->pdcanvas = canvas_getcurrent();
    
    return (void*) (obj->pdobj);
}

void poffont_free(void *x)
{
	pofFont* px= (pofFont*)(((PdObject*)x)->parent);
	delete px;
}

void poffont_set(void *x, t_symbol *file, t_float size)
{
	pofFont* px= (pofFont*)(((PdObject*)x)->parent);
	
	px->fontfile = file;
	px->size = size;
	px->need_reload = true;
}

void pofFont::update()
{
	if(need_reload) {
		t_symbol *file = makefilename(fontfile, pdcanvas);
		if(!file) return;
		offont.loadFont(file->s_name,size, true, true);
		offont.setEncoding(OF_ENCODING_UTF8);
	}
	need_reload = false;
}


//--------- static : ------------

void pofFont::setup(void)
{
	//post("poffont_setup");
	poffont_class = class_new(gensym("poffont"), (t_newmethod)poffont_new, (t_method)poffont_free,
		sizeof(PdObject), 0, A_SYMBOL, A_SYMBOL, A_FLOAT, A_NULL);
	POF_SETUP(poffont_class);
	class_addmethod(poffont_class, (t_method)poffont_set, gensym("set"), A_SYMBOL, A_FLOAT, A_NULL);

}

ofTrueTypeFont* pofFont::getFont(t_symbol* font)
{
	std::map<t_symbol*,pofFont*>::iterator it;
	
	it = fonts.find(font);
	if( (it != fonts.end()) && (it->second->offont.isLoaded()) ) return &(it->second->offont);
	else return NULL;
}
