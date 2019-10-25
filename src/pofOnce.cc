/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofOnce.h"
#include "pofWin.h"
#include "pofHead.h"

t_class *pofonce_class;
bool pofOnce::FORCE_ONCE = false;
std::list<pofOnce*> pofOnce::headOnces;

static void *pofonce_new(t_symbol *sym,int argc, t_atom *argv)
{
	bool continuousForce = false;
	while(argc) {
		if(argv->a_type == A_SYMBOL) {
			t_symbol *argsym = atom_getsymbol(argv);
			if(argsym == gensym("-continuousForce")) {
				continuousForce = true;
			}
		} 
		argc--; argv++;
	}
	pofOnce* obj = new pofOnce(pofonce_class, continuousForce);
	return (void*) (obj->pdobj);
}

static void pofonce_free(void *x)
{
	delete (pofOnce*)(((PdObject*)x)->parent);
}

static void pofonce_bang(void *x)
{
	pofOnce *px = (pofOnce*)(((PdObject*)x)->parent);
	px->trigger = true;
}

static void pofonce_continuousForce(void *x, t_float t)
{
	pofOnce *px = (pofOnce*)(((PdObject*)x)->parent);
	px->continuousForce = (t != 0);
}

static void pofonce_force(void *x)
{
	pofOnce *px = (pofOnce*)(((PdObject*)x)->parent);
	px->force = true;
}

void pofOnce::setup(void)
{
	//post("pofonce_setup");
	pofonce_class = class_new(gensym("pofonce"), (t_newmethod)pofonce_new, (t_method)pofonce_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofonce_class);
	class_addbang(pofonce_class, (t_method)pofonce_bang);
	class_addmethod(pofonce_class, (t_method)pofonce_force, gensym("force"), A_NULL);
	class_addmethod(pofonce_class, (t_method)pofonce_continuousForce, gensym("continuousForce"), A_DEFFLOAT, A_NULL);
	ofAddListener(pofBase::initFrameEvent, &pofOnce::initFrame);
	ofAddListener(pofBase::rebuildEvent, &pofOnce::rebuild);
}

pofOnce::pofOnce(t_class *Class, bool continuousForce0): pofBase(Class),
	trigger(false), force(false), final_trigger(false), final_force(false), continuousForce(continuousForce0)
{
}

pofOnce::~pofOnce()
{
}

void pofOnce::tree_draw()
{
	bool tmp_force_once = FORCE_ONCE;
	if(final_force) {
		FORCE_ONCE = true;
		final_force = false;
	}
	if(final_trigger || FORCE_ONCE) pofBase::tree_draw();
	final_trigger = false;
	FORCE_ONCE = tmp_force_once;
}

bool pofOnce::oncetree_process()
{
	std::list<pofOnce*>::iterator it = onceChildren.begin();

	final_force = false;
	final_trigger = false;

	if(force || continuousForce) {
		final_force = true;
		force = false;
	}
	if(trigger) {
		final_trigger = true;
		trigger = false;
	}
	while(it != onceChildren.end()) {
		if((*it)->oncetree_process()) final_trigger = 1;
		it++;
	}
	return (final_trigger || final_force);
}

void pofOnce::initFrame(ofEventArgs & args)
{
	std::list<pofOnce*>::iterator it = headOnces.begin();
	FORCE_ONCE = false;
	while(it != headOnces.end()) {
		(*it)->oncetree_process();
		it++;
	}
}

static bool pofbase_oncetree_build(pofBase* obj, pofOnce* parent)
{
	std::list<pofBase*>::iterator it = obj->children.begin();
	pofOnce *newparent = parent;
	bool isOnce = (pd_class(&obj->pdobj->x_obj.ob_pd) == pofonce_class);
	
	//if(isBuilt) return false;
	if(isOnce) {
		newparent = (pofOnce*)obj;
		newparent->onceChildren.clear();
		if(parent) {
			parent->onceChildren.push_back(newparent);
			//post("pofonce: adding child");
		}
		else {
			pofOnce::headOnces.push_back(newparent);
			//post("pofonce: adding head");
		}
	}
	
	while(it != obj->children.end()) {
		pofbase_oncetree_build(*it, newparent);
		it++;
	}
	//isBuilt = true;
	return isOnce;
}

void pofOnce::rebuild(ofEventArgs & args)
{
	pofOnce::headOnces.clear();
	if(pofWin::win) {
		std::list<pofHead*>::iterator it = pofHead::pofheads.begin();
	
		while(it != pofHead::pofheads.end()) {
			pofbase_oncetree_build((*it), NULL);
			it++;
		}
	}
}
