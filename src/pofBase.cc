/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBase.h"
#include "pofWin.h"
#include "pofHead.h"
#include "pofImage.h"
#include "pofBlend.h"
#include "setupAll.h"
#include "RWmutex.h"
#include "EventDispatcher.h"

#include "version.h"

#define BUILD_BY_PD

std::list<pofBase*> pofBase::pofobjs;
std::list<pofBase*> pofBase::pofobjsToUpdate;
RWmutex pofBase::treeMutex;
EventDispatcher pofBase::dispatcher;
bool pofBase::needBuild = false;
ofEvent<ofEventArgs> pofBase::reloadTexturesEvent, pofBase::unloadTexturesEvent;
ofEvent<ofEventArgs> pofBase::initFrameEvent;
ofEvent<ofEventArgs> pofBase::rebuildEvent;
deque<t_binbuf*> pofBase::toPdQueue;
deque<std::vector<Any> > pofBase::toPdQueueVec;
t_clock *pofBase::queueClock;
bool pofBase::doRender = true;
bool pofBase::pdProcessesTouchEvents = false;
ofTexture *pofBase::currentTexture = NULL;
std::map<t_symbol*,ofTexture *> pofBase::textures;
int pofBase::watchdogCount = 0;
int pofBase::touchdownCount = 0;

static t_symbol *s_build;
static t_symbol *s_system;
static t_symbol *s_backpressed;
static t_symbol *s_key;

pofBase::pofBase(t_class *Class) { 
	treeMutex.lockW();
	char selfname[32];

	pdobj = (PdObject*)pd_new(Class);
	pdobj->parent = (pofBase*) this;
	
	snprintf(selfname,32 , "pof%p", (void*)this);
	s_self = gensym(selfname);
	pd_bind(&pdobj->x_obj.ob_pd, s_self);
	
	m_out1 = outlet_new(&(pdobj->x_obj), 0);
	
	pofobjs.push_back(this);
	needBuild = true;
	
	tmpToGUIclock = clock_new(&(pdobj->x_obj), (t_method)tryQueueTmpToGUI);
	
	treeMutex.unlockW();
}

pofBase::~pofBase() { 
	treeMutex.lockW();
	pd_unbind(&pdobj->x_obj.ob_pd, s_self);
	pofobjs.remove(this); 
	if (m_out1) outlet_free(m_out1);
	needBuild = true;
	treeMutex.unlockW();
}

void pofBase::detach() {
	treeMutex.lockW();
	pofobjs.remove(this); 
	needBuild = true;
	treeMutex.unlockW();
}

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
	t_binbuf *bb;
	
	while((bb=dequeueToGUI()) != NULL) {
	  if(binbuf_getnatom(bb)) message(binbuf_getnatom(bb), binbuf_getvec(bb));
	  binbuf_free(bb);
	}
	
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
#ifdef DEBUG_TOUCH
		touchdownCount++;
#endif
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
		pd_error(&(pdobj->x_obj),"pof error: duplicate connection discarded.");
		return;
	}
	
	if(parent) {
		// add this to parent's children:
		std::list<pofBase*>::iterator findIter =
			std::find(parent->children.begin(), parent->children.end(), this);
		if(findIter == parent->children.end()) // don't add this twice
			parent->children.push_back(this);
	}
	isBuilt = true;
	
	t_atom ap;
	ap.a_type=A_POINTER;
	ap.a_w.w_gpointer=(t_gpointer *)this;
	outlet_anything(m_out1, s_build, 1, &ap);
	isBuilt = false;
}

bool pofBase::touchtree_build(pofBase *parent)
{
	std::list<pofBase*>::iterator it = children.begin();
	pofBase *newparent = parent;
	bool touchable = isTouchable();
	static int x=0, y=0;

	if(isBuilt) return false;

	if((parent == NULL) || computeTouch(x, y)) newparent = this;

	while(it != children.end()) {
		if((*it)->touchtree_build(newparent)) {
			newparent->touchChildren.push_back(*it);
			touchable = true;
		}
		it++;
	}
	isBuilt = true;
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

void pofBase::queueToGUI(t_symbol *s, int argc, t_atom *argv) // queue to tmpGUI, then call tryQueueTmpToGUI. 
{
	t_binbuf *bb = binbuf_new();
	t_atom at;
	
	if(s) {
	  SETSYMBOL(&at, s);
	  binbuf_add(bb, 1, &at);
	}
	binbuf_add(bb, argc, argv);
	
	tmpToGUIQueue.push_back(bb);
 	tryQueueTmpToGUI(&(pdobj->x_obj));
}

/*static*/ void pofBase::tryQueueTmpToGUI(void *x) // try_lock toGUImutex then copy tmpQueue to toGUIQueue, else delay(0).
{
  pofBase* px= (pofBase*)(((PdObject*)x)->parent);
  
  clock_unset(px->tmpToGUIclock);
  
  if(px->toGUImutex.try_lock()) {
    while(true) {
      if(px->tmpToGUIQueue.size()==0) {
	      px->toGUImutex.unlock();	
	      return;
      }
      t_binbuf *bb = px->tmpToGUIQueue.front();
      px->tmpToGUIQueue.pop_front();
      px->toGUIQueue.push_back(bb);
    }
  } else clock_delay(px->tmpToGUIclock, 0);
}

t_binbuf *pofBase::dequeueToGUI() // to be used by GUI; returns the next binbuf in the queue (null if none);
{		                              // caller must free the binbuf after use.
	toGUImutex.lock();
	
	if(toGUIQueue.size()==0) {
	  toGUImutex.unlock();	
	  return NULL;
  }
  
  t_binbuf *bb = toGUIQueue.front();
  toGUIQueue.pop_front();
  
	toGUImutex.unlock();
	return bb;
}

//-------- static functions : ---------------
	
void pofBase::pof_build(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofBase *obj = ((PdObject*)x)->parent;
	pofBase *parent = NULL;
	
	if (argc==1 && argv->a_type==A_POINTER) {
		parent = (pofBase *)argv->a_w.w_gpointer;
		obj->tree_build(parent);
	}
	
}

void pofBase::buildAll() {
	treeMutex.lockW();
	pofobjsToUpdate.clear();

	std::list<pofBase*>::iterator it = pofobjs.begin();
	while(it != pofobjs.end()) {
		(*it)->reset_tree();
		if((*it)->hasUpdate()) pofobjsToUpdate.push_back(*it);
		it++;
	}
	if(pofWin::win) {
		std::list<pofHead*>::iterator it2 = pofHead::pofheads.begin();
	
		while(it2 != pofHead::pofheads.end()) {
			(*it2)->tree_build(pofWin::win);
			it2++;
		}

		pofWin::win->touchtree_build(NULL);
		ofEventArgs voidEventArgs;
		ofNotifyEvent(rebuildEvent, voidEventArgs);
	}
	
	needBuild = false;
	treeMutex.unlockW();
}

//--------------------------------------------------------------

void pofBase::updateAll() {
    watchdogCount++; // increment watchdog count
}

void pofBase::drawAll() {
#ifndef BUILD_BY_PD
	static bool finalNeedBuild = false;

	if(finalNeedBuild || needBuild) {
		if(needBuild) {				// don't build now
			treeMutex.lockW();
			finalNeedBuild = true;
			needBuild = false;
			treeMutex.unlockW();
		}
		else if(finalNeedBuild) {	// wait nothing more to build
			finalNeedBuild = 0;
			buildAll();
		}
	}
#endif

    ofSetupScreen();
    ofSetBackgroundAuto(false);

	if(doRender) {
		treeMutex.lockR();
		if(!needBuild) {
			// update objects needing it:
			std::list<pofBase*>::iterator it = pofobjsToUpdate.begin();
			while(it != pofobjsToUpdate.end()) {
				(*it)->update();
				it++;
			}

			// init frame:
			ofEventArgs voidEventArgs;
			ofNotifyEvent(initFrameEvent, voidEventArgs);

			// draw everything:
			currentTexture = NULL;
			ofEnableAlphaBlending();
			pofBlend::currentSrcFactor = GL_SRC_ALPHA;
			pofBlend::currentDestFactor = GL_ONE_MINUS_SRC_ALPHA;
			if(pofWin::win) pofWin::win->tree_draw();
		}
		treeMutex.unlockR();
	}
}

void pofBase::touchDownAll(int x, int y, int id) {
	dispatcher.pushEvent(EventData::DOWN, x, y, id);
}

void pofBase::touchMovedAll(int x, int y, int id) {	
	dispatcher.pushEvent(EventData::MOVE, x, y, id);
}

void pofBase::touchUpAll(int x, int y, int id) {
	dispatcher.pushEvent(EventData::UP, x, y, id);
}

/*void pofBase::touchDoubleTapAll(int x, int y, int id) {
	lock();
	if(pofWin::win) pofWin::win->tree_touchDoubleTap(x, y, id);	
	unlock();
}*/

void pofBase::touchCancelAll() {
	dispatcher.pushEvent(EventData::CANCEL);
}

void pofBase::keyPressed(int key){
	t_atom at[3];
	t_binbuf *bb = binbuf_new();

	if(!s_key->s_thing) return; // return if s_key is not bound (nobody listen...)
	SETSYMBOL(&at[0], s_key);
	SETFLOAT(&at[1], key);
	SETFLOAT(&at[2], 1);
	binbuf_add(bb, 3, at);	
	sendToPd(bb);
}

void pofBase::keyReleased(int key){
	t_atom at[3];
	t_binbuf *bb = binbuf_new();

	if(!s_key->s_thing) return; // return if s_key is not bound (nobody listen...)
	SETSYMBOL(&at[0], s_key);
	SETFLOAT(&at[1], key);
	SETFLOAT(&at[2], 0);
	binbuf_add(bb, 3, at);	
	sendToPd(bb);
}

void pofBase::windowResized(int w, int h)
{
	//pofWin::windowResizedAll(w,h);
}

void pofBase::backPressed()
{
	t_binbuf *bb = binbuf_new();
	t_atom at;

	SETSYMBOL(&at, s_system);
	binbuf_add(bb, 1, &at);
	SETSYMBOL(&at, s_backpressed);
	binbuf_add(bb, 1, &at);
	
	sendToPd(bb);
}

//--------------------------------------------------------------
int buildCount = 0;

void dequeueToPdtick(void* nul)
{
	if(pofBase::pdProcessesTouchEvents) pofBase::dispatcher.popEvents();
	while(pofBase::dequeueToPd());
	while(pofBase::dequeueToPdVec());

#ifdef BUILD_BY_PD
	if(pofBase::needBuild) {
		pofBase::buildAll();
		buildCount++;
		//post("buildCount = %d\n", buildCount);
	}
#endif

	pofBase::watchdogCount = 0; // clear watchdog
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
		const char *in = f->s_name + 2;
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

string makefilenameString(t_symbol *f, t_canvas *pdcanvas)
{
	char dirresult[512];
	char *nameresult;
	int fd;
	
	if(f->s_name[0] == '@' && f->s_name[1] == '[') {
		char buf[256];
		int i = 0;
		const char *in = f->s_name + 2;
		while((*in) && (*in) != ']') buf[i++] = *in++;
		if((*in) == ']') {
			buf[i] = 0;
			f = gensym( string(pofBase::getString(buf) + string(&f->s_name[i+3])).c_str());
		}
	}
	
	if(!strncmp(f->s_name, "http", strlen("http"))) return f->s_name;
		
	fd = canvas_open(pdcanvas, f->s_name, "",dirresult, &nameresult, 512, 0);
	close(fd);
	
	if(fd > 0) return string(dirresult)+"/"+string(nameresult);
	else return f->s_name; // NULL
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

bool pofBase::dequeueToPd()
{
	t_binbuf *bb;
	
	if(!toPdMutex.try_lock()) return false;
	
	if(toPdQueue.size()==0) {
		toPdMutex.unlock();	
		return false;
	}

	bb = toPdQueue.front();
	toPdQueue.pop_front();
	toPdMutex.unlock();	
	
	//lock();
	binbuf_eval(bb,0,0,0);
	binbuf_free(bb);
	//unlock();
	
	return true;
}

bool pofBase::dequeueToPdVec()
{
	
	std::vector<Any> vec;
	//toPdMutex.lock();
	if(!toPdMutex.try_lock()) return false;

	if(toPdQueueVec.size()==0) {
		toPdMutex.unlock();	
		return false;
	}
	
	vec = toPdQueueVec.front();
	toPdQueueVec.pop_front();
	toPdMutex.unlock();	
	
	// vec to binbuf :
	//Any *any;
	t_binbuf *bb = binbuf_new();
	t_atom at;
	unsigned int i, l = (unsigned int)vec.size();
	
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

t_class *pof_class;

void *pof_new()
{
    void* obj = pd_new(pof_class);
    return obj;
}

void pofBase::setup() {
	s_build = gensym("pof_build");
	s_system = gensym("SYSTEM");
	s_backpressed = gensym("backPressed");
	s_key = gensym("#pofkey");

	printVersionMessage();
    
	pof_class = class_new(gensym("pof"), (t_newmethod)pof_new, 0, sizeof(PdObject), 0, A_NULL);
	
	setupAll();
		
	queueClock = clock_new(0,(t_method)dequeueToPdtick);
	clock_delay(queueClock,100);
	
	dispatcher.startThread();
}

void pofBase::release() {
	pofImage::release();
}
