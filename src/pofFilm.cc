/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFilm.h"

t_class *poffilm_class;

#define NEXT_FLOAT_ARG(var) if(argc>0) { if(argv->a_type == A_FLOAT) var = atom_getfloat(argv); argv++; argc--; }

void *poffilm_new(t_symbol *s, int argc, t_atom *argv)
{
    float w=0, h=0;
    t_float istexture=0;
    
    if((argc>0) && (argv->a_type == A_SYMBOL)) istexture = 1;
    else {
        NEXT_FLOAT_ARG(w);
        NEXT_FLOAT_ARG(h);
    }
    
    pofFilm* obj = new pofFilm(poffilm_class, w, h, istexture);
    
    if(!istexture){
        floatinlet_new(&obj->pdobj->x_obj, &obj->width);
        floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    }
    
    return (void*) (obj->pdobj);
}

void poffilm_free(void *x)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);

	if(px->player) delete px->player;
	
	delete px;
}

static void poffilm_load(void *x, t_symbol *f)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);
    
    px->file = f;
}

static void poffilm_play(void *x, t_float p)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);
    
    px->playing = p;
}

static void poffilm_goto(void *x, t_float p)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);
    px->gotoFrame = p;  
}

static void poffilm_speed(void *x, t_float s)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);
    px->speed = s;  
}

void pofFilm::setup(void)
{
	//post("poffilm_setup");
	poffilm_class = class_new(gensym("poffilm"), (t_newmethod)poffilm_new, (t_method)poffilm_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_play, gensym("play"), A_FLOAT, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_goto, gensym("goto"), A_FLOAT, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_speed, gensym("speed"), A_FLOAT, A_NULL);
	POF_SETUP(poffilm_class);
}

void pofFilm::draw()
{
	float h = height;
	if(h==0) h = width;

	if(!player) player = new ofVideoPlayer;
	
	if((file != NULL) && (file != loadedFile) ) {
		loadedFile = file;
		//string *f = new string(loadedFile->s_name);
		//player->load(*f);
		
		player->load(loadedFile->s_name);
	}
	
	if(player->isLoaded()) {
		if(gotoFrame>= 0) {
		    player->setFrame(gotoFrame);
		    gotoFrame = -1;
		}
		
		if(speed > -1000) {
		    player->setSpeed(speed);
		    speed = -1000;
		}
		
		if(playing != actualPlaying) {
			actualPlaying = playing;
			if(actualPlaying) player->play();
			else player->stop();
		}	
		
		player->update();
		if(isTexture) {
		    player->bind();
		    pofBase::currentTexture = &player->getTexture();
		}
		else player->draw(-width/2, -height/2, width, height);
	} 
}

void pofFilm::postdraw()
{
	if(isTexture) player->unbind();
}

