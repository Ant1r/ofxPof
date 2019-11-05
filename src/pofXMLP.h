/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "pugixml.hpp"

class pofsubXMLP {

	int refCount;
	t_symbol *name;
	
	static std::map<t_symbol*,pofsubXMLP*> xmls;

	public:
	pugi::xml_document doc;
	pugi::xml_node node;
	
	bool loaded;
	
	pofsubXMLP(t_symbol *n):refCount(1), name(n), loaded(false){
		//xml.addChild("root");
		node = doc.root();
		xmls[n] = this;
	}
	
	~pofsubXMLP() {
		xmls.erase(name);
	}
	
	bool load(t_symbol *file) {
		if(doc.load_file(ofToDataPath(file->s_name).c_str())) {
			loaded = true;
			node = doc.root();
			return true;
		} else return false;
	}
	
	bool save(t_symbol *file) {
		return doc.save_file(ofToDataPath(file->s_name).c_str());
	}
		
	static pofsubXMLP* getXML(t_symbol *name){
		std::map<t_symbol*,pofsubXMLP*>::iterator it;
		it = xmls.find(name);
		if(it!=xmls.end()) {
			it->second->refCount++;
			return it->second;
		}
		else {
			new pofsubXMLP(name);
			return xmls[name];
		}
	}
				
	static void letXML(pofsubXMLP *xml) {
		if(!--xml->refCount) delete xml;
	}

};


class pofXMLP: public pofBase {
	public:
		pofXMLP(t_class *Class, t_symbol *n):pofBase(Class), name(n) {
			sxml = pofsubXMLP::getXML(n);
		}
		
		virtual ~pofXMLP() {
			pofsubXMLP::letXML(sxml);
		}

		static void setup(void);
			
		t_symbol *name;
		pofsubXMLP *sxml;
		t_canvas *pdcanvas;
};



