/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofWin.h"
#include "RWmutex.h"

t_class *pofwin_class;

pofWin *pofWin::win = NULL;
void(*pofWin::open)(void)=NULL;

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

	if(px->init) return;
	px->windowResized(ofGetWindowWidth(),ofGetWindowHeight());
}

void pofwin_open(void *x)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);

	if(pofWin::open) pofWin::open();
}

void pofwin_window(void *x, t_float width, t_float height, t_float fullscreen)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(width<1) width = 1;
	if(height<1) height = 1;
	
	if(px->init) return;
#if (!defined(TARGET_ANDROID) && !(TARGET_OS_IOS))
	pofBase::treeMutex.lockW(); //avoid doing that during the draw

	ofSetWindowShape((int)width,(int)height);
	ofSetFullscreen(fullscreen!=0);
	ofSetWindowShape((int)width,(int)height);
	pofBase::treeMutex.unlockW();
#endif
}

void pofwin_pos(void *x, t_float X, t_float Y)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(px->init) return;
	pofBase::treeMutex.lockW(); //avoid doing that during the draw
	ofSetWindowPosition((int)X,(int)Y);
	pofBase::treeMutex.unlockW();
}

void pofwin_cursor(void *x, t_float cursor)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(px->init) return;
	pofBase::treeMutex.lockW(); //avoid doing that during the draw
	if(cursor!=0) ofShowCursor(); else ofHideCursor();
	pofBase::treeMutex.unlockW();
}

void pofwin_pdProcessesTouchEvents(void *x, t_float val)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(px->init) return;
	pofBase::treeMutex.lockW(); //avoid doing that during the draw
	pofBase::pdProcessesTouchEvents = (val!=0);
	pofBase::treeMutex.unlockW();
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
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(px->init) return;
	ofSetFrameRate(rate);
}

void pofwin_normalizedtextcoords(void *x, t_float enable)
{
	pofWin* px= (pofWin*)(((PdObject*)x)->parent);
	if(px->init) return;
	if(enable != 0) ofEnableNormalizedTexCoords();
	else ofDisableNormalizedTexCoords();
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
	class_addmethod(pofwin_class, (t_method)pofwin_cursor, gensym("cursor"), A_FLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_background, gensym("background"), A_FLOAT, A_FLOAT,A_FLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_pos, gensym("pos"), A_FLOAT, A_FLOAT, 0);
	class_addmethod(pofwin_class, (t_method)pofwin_normalizedtextcoords, gensym("normalizedtextcoords"), A_FLOAT,0);
	class_addmethod(pofwin_class, (t_method)pofwin_build, gensym("build"), A_NULL);
	class_addmethod(pofwin_class, (t_method)pofwin_open, gensym("open"), A_NULL);
	class_addfloat(pofwin_class, pofwin_float);
	
	class_addmethod(pofwin_class, (t_method)pofwin_pdProcessesTouchEvents, gensym("pdProcessesTouchEvents"), A_FLOAT,0);

	//POF_SETUP(pofwin_class);
	if(pofWin::win == NULL) pofWin::win = new pofWin(pofwin_class);
}

//-------------------------------
pofWin::pofWin(t_class *Class):pofBase(Class),init(true), r(0), g(0), b(0){
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
	//if(this==pofWin::win) ofBackground(r,g,b);
	if(init) {
		ofAddListener(ofEvents().windowResized,this,&pofWin::windowResized);
		windowResized(ofGetWindowWidth(),ofGetWindowHeight());
		init = false;
	}
}

void pofWin::draw()
{
	if(this==pofWin::win) ofClear(r,g,b);
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


