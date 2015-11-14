/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBase.h"
#include "pofWin.h"
#include "pofHead.h"
#include "pofRect.h"
#include "pofBang.h"
#include "pofTranslate.h"
#include "pofRotate.h"
#include "pofScale.h"
#include "pofResetMatrix.h"
#include "pofGetMatrix.h"
#include "pofColor.h"
#include "pofTouch.h"
#include "pofFont.h"
#include "pofText.h"
#include "pofFonts.h"
#include "pofTexts.h"
#include "pofImage.h"
#include "pofFbo.h"
#include "pofTouchable.h"
#include "pofVisible.h"
#include "pofScope.h"
#include "pofCirc.h"
#include "pofUtil.h"
#include "pofXML.h"
#include "pofAccel.h"
#include "pofTextbox.h"
#include "pofJSON.h"

#include "version.h"

std::list<pofBase*> pofBase::pofobjs;
std::list<pofBase*> pofobjsToUpdate;
ofMutex pofBase::globMutex;
bool pofBase::needBuild = false;
ofEvent<ofEventArgs> pofBase::reloadTexturesEvent, pofBase::unloadTexturesEvent;
deque<t_binbuf*> pofBase::toPdQueue;
deque<std::vector<Any> > pofBase::toPdQueueVec;
t_clock *pofBase::queueClock;
bool pofBase::doRender = true;

t_symbol *s_build;
t_symbol *s_system;
t_symbol *s_backpressed;

void pofBase::tree_update()
{
	update();
	
	std::list<pofBase*>::iterator it = children.begin();
	
	while(it != children.end()) {
		(*it)->tree_update();
		it++;
	}
}

void pofBase::tree_draw()
{
	draw();
	
	std::list<pofBase*>::iterator it = children.begin();
	
	while(it != children.end()) {
		(*it)->tree_draw();
		it++;
	}
	
	postdraw();
}

bool pofBase::tree_touchMoved(int x, int y, int id)
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	computeTouch(x, y);
	
	while(it != touchChildren.begin()) {
		it--;
		if((*it)->tree_touchMoved(x, y, id)) return true;
	}
	
	return touchMoved(x, y, id);
}

bool pofBase::tree_touchDown(int x, int y, int id)
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	computeTouch(x, y);

	while(it != touchChildren.begin()) {
		it--;
		if((*it)->tree_touchDown(x, y, id)) return true;
	}
	
	return touchDown(x, y, id);
}

bool pofBase::tree_touchUp(int x, int y, int id)
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	computeTouch(x, y);

	while(it != touchChildren.begin()) {
		it--;
		if((*it)->tree_touchUp(x, y, id)) return true;
	}
	
	return touchUp(x, y, id);
}

bool pofBase::tree_touchDoubleTap(int x, int y, int id)
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	computeTouch(x, y);

	while(it != touchChildren.begin()) {
		it--;
		if((*it)->tree_touchDoubleTap(x, y, id)) return true;
	}
	
	return touchDoubleTap(x, y, id);
}

void pofBase::tree_touchCancel()
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	while(it != touchChildren.begin()) {
		it--;
		(*it)->tree_touchCancel();
	}
	
	touchCancel();
}

void pofBase::reset_tree()
{
	isBuilt = /*touchIsBuilt =*/ false;
	children.clear();
	touchChildren.clear(); 
}

void pofBase::tree_build(pofBase *parent)
{
	if(isBuilt) {
		post("pof error : double connection discarded.");
		return;
	}
	
	if(parent) parent->children.push_back(this);
	isBuilt = true;
	
	t_atom ap;
	ap.a_type=A_POINTER;
	ap.a_w.w_gpointer=(t_gpointer *)this;
	outlet_anything(m_out1, s_build, 1, &ap);
}

bool pofBase::touchtree_build(pofBase *parent)
{
	/*if(touchIsBuilt) {
		//post("pof error : double connection discarded."); // already signaled
		return;
	}
	touchIsBuilt = true;*/
	
	std::list<pofBase*>::iterator it = children.begin();
	pofBase *newparent = parent;
	bool touchable = isTouchable();
	static int x=0, y=0;
	if((parent == NULL) || computeTouch(x, y)) newparent = this;

	while(it != children.end()) {
		if((*it)->touchtree_build(newparent)) {
			newparent->touchChildren.push_back(*it);
			touchable = true;
		}
		it++;
	}
	
	return touchable;
}

//-------- Messaging : ---------------

void pofBase::queueToSelfPd(int argc, t_atom *argv)
{
	t_binbuf *bb = binbuf_new();
	t_atom at;

	//SETSYMBOL(&at, s_self);
	SETSYMBOL(&at, s_self);
	binbuf_add(bb, 1, &at);
	binbuf_add(bb, argc, argv);
	
	sendToPd(bb);
}

//-------- static functions : ---------------
	
void pofBase::pof_build(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofBase *obj = ((PdObject*)x)->parent;
	pofBase *parent = NULL;
	
	if (argc==1 && argv->a_type==A_POINTER) {
		parent = (pofBase *)argv->a_w.w_gpointer;
	}
	
	obj->tree_build(parent);
}


void pofBase::updateAll() {
	
	/*if(doRender) {
		lock();
		std::list<pofBase*>::iterator it = pofobjs.begin();
	
		while(it != pofobjs.end()) {
			(*it)->update();
			it++;
		}
		unlock();
	}*/
	if(doRender) {
		lock();
		if(needBuild) buildAll();
		
		std::list<pofBase*>::iterator it = pofobjsToUpdate.begin();
	
		while(it != pofobjsToUpdate.end()) {
			(*it)->update();
			it++;
		}
		unlock();
	}
}

void pofBase::drawAll(){
	if(doRender) {
		lock();
		if(needBuild) buildAll();
		ofEnableAlphaBlending(); 
		if(pofWin::win) pofWin::win->tree_draw();
		unlock();
	}
    ofSetupScreen();
}

void pofBase::buildAll() {
	pofobjsToUpdate.clear();
	
	std::list<pofBase*>::iterator it = pofobjs.begin();
	while(it != pofobjs.end()) {
		(*it)->reset_tree();
		//if(&((*it)->update) != &pofBase::update) pofobjsToUpdate.push_back(*it);
		if((*it)->hasUpdate()) pofobjsToUpdate.push_back(*it);
		it++;
	}
	//cout<<"nb obj to update:"<<pofobjsToUpdate.size()<<endl;
	if(pofWin::win) {
		std::list<pofHead*>::iterator it2 = pofHead::pofheads.begin();
	
		while(it2 != pofHead::pofheads.end()) {
			(*it2)->tree_build(pofWin::win);
			it2++;
		}

		pofWin::win->touchtree_build(NULL);
	}
	
	needBuild = false;
}

void pofBase::touchDownAll(int x, int y, int id) {
	lock();
	if(pofWin::win) pofWin::win->tree_touchDown(x, y, id);	
	unlock();
}

void pofBase::touchMovedAll(int x, int y, int id) {	
	lock();
	if(pofWin::win) pofWin::win->tree_touchMoved(x, y, id);	
	unlock();
}

void pofBase::touchUpAll(int x, int y, int id) {
	lock();
	if(pofWin::win) pofWin::win->tree_touchUp(x, y, id);
	unlock();
}

/*void pofBase::touchDoubleTapAll(int x, int y, int id) {
	lock();
	if(pofWin::win) pofWin::win->tree_touchDoubleTap(x, y, id);	
	unlock();
}*/

void pofBase::touchCancelAll() {
	lock();
	if(pofWin::win) pofWin::win->tree_touchCancel();
	unlock();
}

void pofBase::windowResized(int w, int h)
{
	//pofWin::windowResizedAll(w,h);
}

void pofBase::backPressed()
{
	t_binbuf *bb = binbuf_new();
	t_atom at;

	//SETSYMBOL(&at, s_self);
	SETSYMBOL(&at, s_system);
	binbuf_add(bb, 1, &at);
	SETSYMBOL(&at, s_backpressed);
	binbuf_add(bb, 1, &at);
	
	sendToPd(bb);
}

void dequeueToPdtick(void* nul)
{
	while(pofBase::dequeueToPd());
	while(pofBase::dequeueToPdVec());
	//(pofBase::dequeueToPd());
	//(pofBase::dequeueToPdVec());
	clock_delay(pofBase::queueClock,2); //poll events every 2ms
}

t_symbol *makefilename(t_symbol *f, t_canvas *pdcanvas)
{
	char dirresult[512];
	char *nameresult;
	int fd;
	
	if(f->s_name[0] == '@' && f->s_name[1] == '[') {
		char buf[256];
		int i = 0;
		char *in = f->s_name + 2;
		while((*in) && (*in) != ']') buf[i++] = *in++;
		if((*in) == ']') {
			buf[i] = 0;
			f = gensym( string(pofBase::getString(buf) + string(&f->s_name[i+3])).c_str());
		}
	}
	
	if(!strncmp(f->s_name, "http", strlen("http"))) return f;
		
	fd = canvas_open(pdcanvas, f->s_name, "",dirresult, &nameresult, 512, 0);
	close(fd);
	
	if(fd > 0) return gensym((string(dirresult)+"/"+string(nameresult)).c_str());
	else return f; // NULL
}

void pofBase::reloadTextures()
{
	ofEventArgs voidEventArgs;
	ofNotifyEvent( reloadTexturesEvent, voidEventArgs );
}

void pofBase::unloadTextures()
{
	ofEventArgs voidEventArgs;
	ofNotifyEvent( unloadTexturesEvent, voidEventArgs );
}

ofMutex toPdMutex;

void pofBase::sendToPd(t_binbuf *bb)
{
	toPdMutex.lock();
	toPdQueue.push_back(bb);
	toPdMutex.unlock();	
}

void pofBase::sendToPd(std::vector<Any> &vec)
{
	toPdMutex.lock();
	toPdQueueVec.push_back(vec);
	toPdMutex.unlock();	
}

/*void pofBase::sendToPd(...)
{
	sendToPd(args);
}*/

int pofBase::dequeueToPd()
{
	t_binbuf *bb;
	
	toPdMutex.lock();
	if(toPdQueue.size()==0) {
		toPdMutex.unlock();	
		return 0;
	}

	bb = toPdQueue.front();
	toPdQueue.pop_front();
	toPdMutex.unlock();	
	
	//lock();
	binbuf_eval(bb,0,0,0);
	binbuf_free(bb);
	//unlock();
	
	return 1;
}

int pofBase::dequeueToPdVec()
{
	
	toPdMutex.lock();
	if(toPdQueueVec.size()==0) {
		toPdMutex.unlock();	
		return 0;
	}

	std::vector<Any> vec;
	t_binbuf *bb = binbuf_new();
	t_atom at;
	
	vec = toPdQueueVec.front();
	toPdQueueVec.pop_front();
	toPdMutex.unlock();	
	
	// vec to binbuf :
	//Any *any;
	unsigned int i, l = vec.size(); 
	for (i=0 ; i<l ; i++) {
		Any *any = &vec[i];
		if(any->type() == typeid(string)) {
			SETSYMBOL(&at, gensym(RefAnyCast<string>(*any).c_str()) );
			binbuf_add(bb, 1, &at);
		}
		else if(any->type() == typeid(float) || any->type() == typeid(int)) {
			SETFLOAT(&at, RefAnyCast<float>(*any));
			binbuf_add(bb, 1, &at);
		}
	}
	
	//lock();
	binbuf_eval(bb,0,0,0);
	binbuf_free(bb);
	//unlock();
	
	return 1;
}

map<string, string> stringsMap;

void pofBase::setString(const string& id, const string& value)
{
	stringsMap[id] = value;
}

string pofBase::getString(const string& id)
{
	return stringsMap[id];
}
//--------------------- SETUPs -----------------
#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)

const char* versionString() {
return ( "" STRINGIFY(POF_VERSION_MAJOR) "." STRINGIFY(POF_VERSION_MINOR) "." STRINGIFY(POF_VERSION_PATCH));
}



void pofBase::setup() {
	s_build = gensym("pof_build");
	s_system = gensym("SYSTEM");
	s_backpressed = gensym("backPressed");

	post(		"Pof : openFrameworks for Pure Data.");
	verbose(-1, " Pof %s compiled on " __DATE__, versionString());
    verbose(-1, " Pof (BSD) 2014 Antoine Rousseau @ metalu.net");
	
	pofWin::setup();
	pofHead::setup();
	pofRect::setup();
	pofBang::setup();
	pofTranslate::setup();
	pofRotate::setup();
	pofScale::setup();
	pofResetMatrix::setup();
	pofGetMatrix::setup();
	pofColor::setup();
	pofTouch::setup();
	pofTouchable::setup();
	pofVisible::setup();
	pofFont::setup();
	pofText::setup();
	pofFonts::setup();
	pofTexts::setup();
	pofImage::setup();
	pofFbo::setup();
	pofScope::setup();
	pofCirc::setup();
	pofUtil::setup();
	pofXML::setup();
	pofAccel::setup();
	pofTextbox::setup();
	pofJSON::setup();
		
	queueClock = clock_new(0,(t_method)dequeueToPdtick);
	clock_delay(queueClock,100);
}


