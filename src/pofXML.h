/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofsubXML {

	int refCount;
	t_symbol *name;
	
	static std::map<t_symbol*,pofsubXML*> xmls;

	ofMutex mutex;

	public:
	ofXml xml;
	bool loaded;
	
	pofsubXML(t_symbol *n):refCount(1), name(n), loaded(false){
		xml.addChild("root");
		xmls[n] = this;
	}
	
	~pofsubXML() {
		xmls.erase(name);
	}
	
	bool load(t_symbol *file) {
		mutex.lock();
		if(xml.load(file->s_name)) {
			loaded = true;
			xml.reset();
			mutex.unlock();
			return true;
		} else {
			mutex.unlock();
			return false;
		}
	}

	static pofsubXML* getXML(t_symbol *name){
		std::map<t_symbol*,pofsubXML*>::iterator it;
		it = xmls.find(name);
		if(it!=xmls.end()) {
			it->second->refCount++;
			return it->second;
		}
		else {
			new pofsubXML(name);
			return xmls[name];
		}
	}
				
	static void letXML(pofsubXML *xml) {
		if(!--xml->refCount) delete xml;
	}

};


class pofXML: public pofBase {
	public:
		pofXML(t_class *Class, t_symbol *n):pofBase(Class), name(n) {
			sxml = pofsubXML::getXML(n);
		}
		
		virtual ~pofXML() {
			pofsubXML::letXML(sxml);
		}

		static void setup(void);
			
		t_symbol *name;
		pofsubXML *sxml;
		t_canvas *pdcanvas;
};



