/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFbo.h"

t_class *poffbo_class;

std::map<t_symbol*,pofsubFbo*> pofsubFbo::sfbos;

pofsubFbo::pofsubFbo(t_symbol *n):refCount(1), name(n), width(0), height(0) {
	sfbos[n] = this;
	ofAddListener(pofBase::reloadTexturesEvent, this, &pofsubFbo::reloadTexture);
	//ofAddListener(pofBase::unloadTexturesEvent, this, &pofsubFbo::unloadTexture);
	pofBase::textures[name] = &fbo.getTexture();
}
	
pofsubFbo::~pofsubFbo() {
	sfbos.erase(name);
	ofRemoveListener(pofBase::reloadTexturesEvent, this, &pofsubFbo::reloadTexture);	
	//ofRemoveListener(pofBase::unloadTexturesEvent, this, &pofFbo::unloadTexture);
	pofBase::textures.erase(name);
}

void pofsubFbo::reloadTexture(ofEventArgs & args) { 
	fbo.allocate(width, height, GL_RGBA); 
	pofBase::textures[name] = &fbo.getTexture();
}

pofsubFbo* pofsubFbo::get(t_symbol *name){
	std::map<t_symbol*,pofsubFbo*>::iterator it;
	it = sfbos.find(name);
	if(it!=sfbos.end()) { //fbo found
		it->second->refCount++;
		return it->second;
	}
	else { // fbo not found ; create a new one.
		new pofsubFbo(name);
		return sfbos[name];
	}
}
			
void pofsubFbo::let(pofsubFbo *sfbo) {
	if(!--sfbo->refCount) delete sfbo;
}

void pofsubFbo::begin(float w, float h) {
    if(w < 0) w = 0;
	if(h < 0) h = 0;
	
	if(w != 0) width = w;
	if(h != 0) height = h;
	
	if(width != 0 && height != 0) {
		if( (!fbo.isAllocated()) || (fbo.getWidth() != width) || (fbo.getHeight() != height))
			fbo.allocate(width, height, GL_RGBA);
	}
	fbo.begin();
	pofBase::textures[name] = &fbo.getTexture();
}

void pofsubFbo::end() {
	fbo.end();
}

void pofsubFbo::draw(float w, float h) {
	if(fbo.isAllocated()) fbo.draw(-width/2,-height/2,width,height);
}


/*******************************************/


void *poffbo_new(t_symbol *sym,int argc, t_atom *argv)
{
    float w = 0, h = 0;
    t_symbol *name = NULL;
    
    if(argc && argv->a_type == A_SYMBOL) {
    	name = atom_getsymbol(argv); argc--; argv++;   	
    	if(argc && argv->a_type == A_FLOAT) {
    		w = atom_getfloat(argv); argc--; argv++; 
    		if(argc && argv->a_type == A_FLOAT) h = atom_getfloat(argv); argc--; argv++;
    	}
    } else if(argc && argv->a_type == A_FLOAT) {
    	w = atom_getfloat(argv); argc--; argv++; 
    	if(argc && argv->a_type == A_FLOAT) {
    		h = atom_getfloat(argv); argc--; argv++;
    		if(argc && argv->a_type == A_SYMBOL) name = atom_getsymbol(argv); argc--; argv++;
    	}
    }
    
    pofFbo* obj = new pofFbo(poffbo_class, w, h);
    
    if(name == NULL) name = obj->s_self;
    	
    obj->sfbo = pofsubFbo::get(name);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    return (void*) (obj->pdobj);
}

void poffbo_free(void *x)
{
	pofFbo *px = (pofFbo*)(((PdObject*)x)->parent);
	//if(px->sfbo) pofsubFbo::let(px->sfbo);
	delete px;
}

void poffbo_float(void *x, t_float t)
{
	pofFbo *px = (pofFbo*)(((PdObject*)x)->parent);
	
	px->update = (t!=0);
}

void poffbo_clear(void *x, t_float c)
{
	pofFbo *px = (pofFbo*)(((PdObject*)x)->parent);
	
	px->clear = (c!=0);
}

void poffbo_set(void *x, t_symbol *name)
{
	pofFbo *px = (pofFbo*)(((PdObject*)x)->parent);
	
	pofsubFbo::let(px->sfbo);
	px->sfbo = pofsubFbo::get(name);
}

void pofFbo::setup(void)
{
	//post("poffbo_setup");
	poffbo_class = class_new(gensym("poffbo"), (t_newmethod)poffbo_new, (t_method)poffbo_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addfloat(poffbo_class, (t_method)poffbo_float);
	class_addmethod(poffbo_class, (t_method)poffbo_clear, gensym("clear"), A_FLOAT, A_NULL);	
	class_addmethod(poffbo_class, (t_method)poffbo_set, gensym("set"), A_SYMBOL, A_NULL);
	POF_SETUP(poffbo_class);
}

void pofFbo::draw()
{
	sfbo->begin(width, height);
	if(update) {
		if(clear) ofClear(255,255,255, 0);
		ofTranslate(sfbo->width/2, sfbo->height/2);
	}
}

void pofFbo::postdraw()
{
	sfbo->end();
	sfbo->draw(width, height);
}

void pofFbo::tree_draw()
{
	if(update) pofBase::tree_draw();
	else sfbo->draw(width, height);
}

bool pofFbo::tree_touchDown(int x, int y, int id)
{
	std::list<pofBase*>::iterator it = touchChildren.end();
	
	computeTouch(x, y);

	if ((x>width/2)||(x<-width/2)||(y>height/2)||(y<-height/2)) return false;
	
	while(it != touchChildren.begin()) {
		it--;
		if((*it)->tree_touchDown(x, y, id)) return true;
	}
	
	return touchDown(x, y, id);
}

