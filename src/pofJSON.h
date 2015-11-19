/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "ofxJSON.h"

class pofsubJSON {

	int refCount;
	t_symbol *name;
	
	static std::map<t_symbol*,pofsubJSON*> jsons;

	ofMutex mutex;

	public:
	ofxJSONElement json;
	bool loaded;
	
	pofsubJSON(t_symbol *n):refCount(1), name(n), loaded(false){
		jsons[n] = this;
	}
	
	~pofsubJSON() {
		jsons.erase(name);
	}
	
	bool load(t_symbol *file) {
		mutex.lock();
		loaded = false;
		mutex.unlock();
		
		if(json.open(file->s_name)) {
			mutex.lock();
			loaded = true;
			mutex.unlock();
			return true;
		} else {
			//mutex.unlock();
			return false;
		}
	}

	ofxJSONElement get(int argc, t_atom *argv)
	{
		if(!loaded) return ofxJSONElement();
		ofxJSONElement js = json;
		
		try {
			while(argc>0) {
				if(js.empty()) return ofxJSONElement();
				if(argv->a_type == A_SYMBOL) js = js[atom_getsymbol(argv)->s_name];
				else if(argv->a_type == A_FLOAT) {
					if(!js.isArray()) return ofxJSONElement();
					else js = js[(unsigned int)atom_getfloat(argv)];
				}
				argc--; argv++;
			}
			return js;	
		}
		catch (int e) { return ofxJSONElement(); }
	}
		
	static pofsubJSON* getJSON(t_symbol *name){
		std::map<t_symbol*,pofsubJSON*>::iterator it;
		it = jsons.find(name);
		if(it!=jsons.end()) {
			it->second->refCount++;
			return it->second;
		}
		else {
			new pofsubJSON(name);
			return jsons[name];
		}
	}
				
	static void letJSON(pofsubJSON *json) {
		if(!--json->refCount) delete json;
	}

};

class JSONLoader;

class pofJSON: public pofBase {
	public:
		pofJSON(t_class *Class, t_symbol *n):pofBase(Class), loader(NULL) {
				sjson = pofsubJSON::getJSON(n);
		}
		
		virtual ~pofJSON() {
			pofsubJSON::letJSON(sjson);
		}

		static void setup(void);
			
		pofsubJSON *sjson;
		JSONLoader *loader;
		t_canvas *pdcanvas;
};



