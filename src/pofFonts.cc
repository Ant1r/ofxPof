/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFonts.h"

t_class *poffonts_class;

std::map<t_symbol*,pofFonts*> pofFonts::fonts;

void *poffonts_new(t_symbol *font, t_symbol *fontfile)
{
	if(pofFonts::fonts.find(font)!=pofFonts::fonts.end()) {
    	post("poffonts error : duplicate font name %s",font->s_name);
    	return NULL;
    }
    
    pofFonts* obj = new pofFonts(poffonts_class, font, fontfile);
    obj->pdcanvas = canvas_getcurrent();
    
    return (void*) (obj->pdobj);
}

void poffonts_free(void *x)
{
	pofFonts* px= (pofFonts*)(((PdObject*)x)->parent);
	delete px;
}

void poffonts_set(void *x, t_symbol *file)
{
	pofFonts* px= (pofFonts*)(((PdObject*)x)->parent);
	
	px->fontfile = file;
	px->need_reload = true;
}

void pofFonts::update()
{
	if(need_reload) {
		
		t_symbol *file = makefilename(fontfile, pdcanvas);
		if(!file) return;
		
		// FontStash's mipmaping currently doesn't work for Android...
		/*offont.setup(fileName, //font file, ttf only
				  1.0,					//lineheight percent
				  1024,					//texture atlas dimension
				  true,					//create mipmaps of the font, useful to scale down the font at smaller sizes
				  8,					//texture atlas element padding, shouldbe >0 if using mipmaps otherwise
				  2.0f					//dpi scaleup, render textures @2x the reso
				  );					//lower res mipmaps wil bleed into each other*/


		offont.clear();
		offont.setup(file->s_name, //font file, ttf only
				  1.0					//lineheight percent
				  );					//lower res mipmaps wil bleed into each other*/
	}
	need_reload = false;
}

void pofFonts::reloadTexture(ofEventArgs & args){
	need_reload = true;
}

void pofFonts::unloadTexture(ofEventArgs & args){
	offont.clear();
}

//--------- static : ------------

void pofFonts::setup(void)
{
	//post("poffonts_setup");
	poffonts_class = class_new(gensym("poffonts"), (t_newmethod)poffonts_new, (t_method)poffonts_free,
		sizeof(PdObject), 0, A_SYMBOL, A_SYMBOL, A_NULL);
	POF_SETUP(poffonts_class);
	class_addmethod(poffonts_class, (t_method)poffonts_set, gensym("set"), A_SYMBOL, A_NULL);

}

ofxFontStash* pofFonts::getFont(t_symbol* font)
{
	std::map<t_symbol*,pofFonts*>::iterator it;
	
	it = fonts.find(font);
	if( (it != fonts.end()) && (it->second->offont.isLoaded()) ) return &(it->second->offont);
	else return NULL;
}
