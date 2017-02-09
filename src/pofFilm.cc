/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFilm.h"

static t_class *poffilm_class;

#define NEXT_FLOAT_ARG(var) if((argc>0)&&(argv->a_type == A_FLOAT)) { var = atom_getfloat(argv); argv++; argc--; }
static t_symbol *s_size, *s_frame;

static void *poffilm_new(t_symbol *s, int argc, t_atom *argv)
{
    float w=0, h=0;
    t_float istexture=0;
    t_symbol *name = NULL;
    
    if((argc>0) && (argv->a_type == A_SYMBOL)) {
    	istexture = 1;
    	name = atom_getsymbol(argv);
	}
    else {
        NEXT_FLOAT_ARG(w);
        NEXT_FLOAT_ARG(h);
        if((argc>0) && (argv->a_type == A_SYMBOL)) name = atom_getsymbol(argv);
    }
    
    pofFilm* obj = new pofFilm(poffilm_class, w, h, istexture);
    
    if(!istexture){
        floatinlet_new(&obj->pdobj->x_obj, &obj->width);
        floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    } 
    obj->name = name;

    obj->pdcanvas = canvas_getcurrent();

	obj->m_out2 = outlet_new(&(obj->pdobj->x_obj), 0);

    return (void*) (obj->pdobj);
}

static void poffilm_free(void *x)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);	
	delete px;
}

static void poffilm_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
    pofFilm* px = (pofFilm*)(((PdObject*)x)->parent);

    outlet_anything(px->m_out2, s, argc, argv);
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
	s_size = gensym("size");
	s_frame = gensym("frame");
	poffilm_class = class_new(gensym("poffilm"), (t_newmethod)poffilm_new, (t_method)poffilm_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_out, s_size, A_GIMME, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_out, s_frame, A_GIMME, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_play, gensym("play"), A_FLOAT, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_goto, gensym("goto"), A_FLOAT, A_NULL);
	class_addmethod(poffilm_class, (t_method)poffilm_speed, gensym("speed"), A_FLOAT, A_NULL);
	POF_SETUP(poffilm_class);
}

#include <locale.h>

void pofFilm::draw()
{
	float h = height;
	//char* currentLocale = setlocale(LC_ALL, NULL); // pointer to store current locale
	if(h==0) h = width;

#ifndef RASPI
//	if(!player) player = new ofVideoPlayer;
#endif		
	
	if((file != NULL) && (file != loadedFile) ) {
		loadedFile = file;
		actualPlaying = 0;
#ifdef RASPI
	    if(player) delete player;
		player = new ofxOMXPlayer;
		ofxOMXPlayerSettings settings;
	    settings.videoPath = makefilename(loadedFile, pdcanvas)->s_name;//loadedFile->s_name;
	    //settings.useHDMIForAudio = true;	//default true
	    settings.enableTexture = true;		//default true
	    settings.enableLooping = true;		//default true
	    settings.enableAudio = false;		//default true, save resources by disabling
        player->setup(settings);
		//string *f = new string(loadedFile->s_name);
		//player->load(*f);
		 
#else
	    if(player) delete player;
	    player = new ofVideoPlayer;
		player->loadMovie(makefilename(loadedFile, pdcanvas)->s_name/*loadedFile->s_name*/);
#endif	
        setlocale(LC_ALL, "C"); // WHY DO I HAVE TO DO THAT ??????????? OF changes the locale when loading a video...
                              // Without this fix, on a french computer pd starts stringing floating numbers with a comma,
                              // e.g. "0,5" which breaks the communication with TclTk GUI and other network connected programs.
		if(player) {
			if(name) pofBase::textures[name] = &player->getTexture();
			t_atom ap[4];
			float w = player->getWidth(), h = player->getHeight(), len = player->getTotalNumFrames();
			SETSYMBOL(&ap[0], s_size);
			SETFLOAT(&ap[1], w);
			SETFLOAT(&ap[2], h);
			SETFLOAT(&ap[3], len);
			queueToSelfPd(4, ap);
		}
	}
	
	if(!player) return;
	
#ifndef RASPI	
	if(player->isLoaded()) {
		if(gotoFrame>= 0) {
		    player->setFrame(gotoFrame);
		    gotoFrame = -1;
		    //setlocale(LC_ALL, currentLocale);
		}
		
		if(speed > -1000) {
		    player->setSpeed(speed);
		    speed = -1000;
		    //setlocale(LC_ALL, currentLocale);
		}
		
		if(playing != actualPlaying) {
			actualPlaying = playing;
			if(actualPlaying) player->play();
			else player->stop();
			//setlocale(LC_ALL, currentLocale);
		}	
		player->update();
		
#else
    if(player->getIsOpen()) {
		if(playing != actualPlaying) {
			actualPlaying = playing;
			player->setPaused(!playing);
		}	
#endif	
        int i = player->getCurrentFrame();
        if(i != currentFrame) {
            currentFrame = i;
			t_atom ap[2];
			float w = player->getWidth(), h = player->getHeight(), len = player->getTotalNumFrames();
			SETSYMBOL(&ap[0], s_frame);
			SETFLOAT(&ap[1], currentFrame);
			queueToSelfPd(2, ap);
        }   	
		
		if(isTexture) {
		    player->getTextureReference().bind();
		    pofBase::currentTexture = &player->getTextureReference();
		}
		else {
			float h = height;
			if(!h) h = width * player->getHeight() / player->getWidth(); 
			player->draw(-width/2, -h/2, width, h);
		}
	}
	//setlocale(LC_ALL, "C");
}

void pofFilm::postdraw()
{
	if(isTexture && (player!=NULL)) player->getTextureReference().unbind();
}

