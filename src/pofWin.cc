/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofWin.h"

t_class *pofwin_class;

pofWin *pofWin::win = NULL;
static t_symbol *s_out, *s_window;

void *pofwin_new(void)
{
    pofWin* obj = new pofWin(pofwin_class);
    return (void*) (obj->pdobj);
}

void pofwin_free(void *x)
{
	delete (pofWin*)(((PdObject*)x)->parent);
}

void pofwin_float(void *x, t_floatarg f)
{
    pofBase::doRender = (f!=0);
    x=NULL; /* don't warn about unused variables */
}

void pofwin_bang(void *x)
{
    pofWin* px= (pofWin*)(((PdObject*)x)->parent);

	px->windowResized(ofGetWindowWidth(),ofGetWindowHeight());
}

void pofwin_window(void *x, t_float width, t_float height, t_float fullscreen)
{
	x=NULL; /* don't warn about unused variables */
	if(width<1) width = 1;
	if(height<1) height = 1;
	
	pofBase::lock();

	ofSetWindowShape((int)width,(int)height);

	pofBase::unlock();
}

void pofwin_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);

	if((argc>1) && argv->a_type == A_SYMBOL)
		outlet_anything(px->m_out1, atom_getsymbol(argv), argc-1, argv+1);
}

void pofwin_background(void *x, t_float _r, t_float _g, t_float _b)
{
	pofWin::win->r = _r;
	pofWin::win->g = _g;
	pofWin::win->b = _b;
}

void pofwin_framerate(void *x, t_float rate)
{
	ofSetFrameRate(rate);
	x=NULL; /* don't warn about unused variables */
}

void pofwin_build(void *x)
{
	pofBase::needBuild = true;
	x=NULL; /* don't warn about unused variables */
}

void pofWin::setup(void)
{
	//post("pofwin_setup");
	s_window = gensym("window");
	s_out = gensym("out");
	
	pofwin_class = class_new(gensym("pofwin"), (t_newmethod)pofwin_new, (t_method)pofwin_free,
		sizeof(PdObject), 0, (t_atomtype)0);

	class_addbang(pofwin_class, (t_method)pofwin_bang);
	class_addmethod(pofwin_class, (t_method)pofwin_window, s_window, A_FLOAT, A_FLOAT,A_DEFFLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_out, s_out, A_GIMME, 0);
	class_addmethod(pofwin_class, (t_method)pofwin_framerate, gensym("framerate"), A_FLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_background, gensym("background"), A_FLOAT, A_FLOAT,A_FLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_build, gensym("build"), A_NULL);
	class_addfloat(pofwin_class, pofwin_float);

	//POF_SETUP(pofwin_class);
	if(pofWin::win == NULL) pofWin::win = new pofWin(pofwin_class);
}

//-------------------------------
pofWin::pofWin(t_class *Class):pofBase(Class),init(true){
	//ofAddListener(ofEvents().windowResized,this,&pofWin::windowResized);
}

pofWin::~pofWin() { 
	ofRemoveListener(ofEvents().windowResized,this,&pofWin::windowResized);
}

void pofWin::windowResized(ofResizeEventArgs & resize){
	windowResized(resize.width,resize.height);
}

void pofWin::update()
{
	if(this==pofWin::win) ofBackground(r,g,b);
	if(init) {
		ofAddListener(ofEvents().windowResized,this,&pofWin::windowResized);
		windowResized(ofGetWindowWidth(),ofGetWindowHeight());
		init = false;
	}
}

bool pofWin::computeTouch(int &x, int &y)
{
	x -= ofGetWindowWidth()/2;
	y -= ofGetWindowHeight()/2;
	return true;
}

void pofWin::windowResized(int w, int h)
{
	t_atom ap[4];
	
	SETSYMBOL(&ap[0], s_out);
	SETSYMBOL(&ap[1], s_window);
	SETFLOAT(&ap[2], w);
	SETFLOAT(&ap[3], h);

	queueToSelfPd(4, ap);
}


