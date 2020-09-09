/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofIPcam.h"

static t_class *pofIPcam_class;

#define NEXT_FLOAT_ARG(var) if((argc>0)&&(argv->a_type == A_FLOAT)) { var = atom_getfloat(argv); argv++; argc--; }
static t_symbol *s_size, *s_connect, *s_disconnect, *s_connected;


static void *pofIPcam_new(t_symbol *s, int argc, t_atom *argv)
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

	pofIPcam* obj = new pofIPcam(pofIPcam_class, w, h, istexture);

	if(!istexture){
		floatinlet_new(&obj->pdobj->x_obj, &obj->width);
		floatinlet_new(&obj->pdobj->x_obj, &obj->height);
	}
	obj->name = name;

	obj->pdcanvas = canvas_getcurrent();

	obj->m_out2 = outlet_new(&(obj->pdobj->x_obj), 0);

	return (void*) (obj->pdobj);
}

static void pofIPcam_free(void *x)
{
	pofIPcam* px = (pofIPcam*)(((PdObject*)x)->parent);
	px->cam.waitForDisconnect();
	delete px;
}

static void pofIPcam_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofIPcam* px = (pofIPcam*)(((PdObject*)x)->parent);

	outlet_anything(px->m_out2, s, argc, argv);
}

void pofIPcam::setup(void)
{
	s_size = gensym("size");
	s_connect = gensym("connect");
	s_disconnect = gensym("disconnect");
	s_connected = gensym("connected");
	
	pofIPcam_class = class_new(gensym("pofipcam"), (t_newmethod)pofIPcam_new, (t_method)pofIPcam_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofIPcam_class, (t_method)pofIPcam_out, s_size,			A_GIMME, A_NULL);
	class_addmethod(pofIPcam_class, (t_method)pofIPcam_out, s_connected,	A_GIMME, A_NULL);
	class_addmethod(pofIPcam_class, (t_method)tellGUI, 		s_connect,		A_GIMME, A_NULL);
	class_addmethod(pofIPcam_class, (t_method)tellGUI, 		s_disconnect,	A_GIMME, A_NULL);

	POF_SETUP(pofIPcam_class);
}

#include <locale.h>

void pofIPcam::draw()
{
//#ifndef TARGET_ANDROID
	int i;
	float h = height;
	if(h==0) h = width;
	
	cam.update();
	if(!isConnected && cam.isConnected()) {
		t_atom ap[4];
		SETSYMBOL(&ap[0], s_connected);
		SETFLOAT(&ap[1], 1);
		queueToSelfPd(2, ap);
		SETSYMBOL(&ap[0], s_size);
		SETFLOAT(&ap[1], cam.getWidth());
		SETFLOAT(&ap[2], cam.getHeight());
		queueToSelfPd(3, ap);
		isConnected = true;
	}

	if(name) pofBase::textures[name] = &cam.getTexture();

	if(isTexture) {
		cam.getTexture().bind();
		pofBase::currentTexture = &cam.getTexture();
	}
	else {
		float h = height;
		if(!h) h = width * cam.getHeight() / cam.getWidth();
		cam.draw(-width/2, -h/2, width, h);
	}
}

void pofIPcam::postdraw()
{
	if(isTexture) cam.getTexture().unbind();
}

void pofIPcam::connect()
{
	if(url != NULL) {
		disconnect();
		cam.setURI(url->s_name);
		cam.connect();
		url = NULL;
	}
}

void pofIPcam::disconnect()
{
	if(isConnected) {
		cam.waitForDisconnect();
		cam.reset();
		t_atom ap[3];
		SETSYMBOL(&ap[0], s_connected);
		SETFLOAT(&ap[1], 0);
		queueToSelfPd(2, ap);
		isConnected = false;
	}
}

void pofIPcam::message(int argc, t_atom *argv)
{
	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;

	if(key == s_connect) {
		if(argc && argv->a_type == A_SYMBOL) {
			url = atom_getsymbol(argv);
			connect();
		}
	}
	else if(key == s_disconnect){
		disconnect();
	}
}
