/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofJSON.h"

std::map<t_symbol*,pofsubJSON*> pofsubJSON::jsons;

t_class *pofjson_class;
t_symbol *s_out,*s_errload,*s_loaded;

class JSONLoader: public ofThread {
	public :
    t_symbol *file, *fullfile;
    pofJSON* pjson;
    
	JSONLoader(t_symbol *f, t_symbol *ff, pofJSON* pj) : file(f), fullfile(ff), pjson(pj) {}
	void threadedFunction(){
		t_atom at[3];
		
		SETSYMBOL(&at[0], s_out);
		SETSYMBOL(&at[2], file);
	
		if(fullfile && pjson->sjson->load(fullfile)) SETSYMBOL(&at[1], s_loaded);
		else SETSYMBOL(&at[1], s_errload);
	
		pjson->queueToSelfPd(3, at);
		pjson->loader = NULL;
    }
};

void *pofjson_new(t_symbol *n)
{
    pofJSON* obj = new pofJSON(pofjson_class, n);
    
    obj->pdcanvas = canvas_getcurrent();
    
    return (void*) (obj->pdobj);
}

void pofjson_free(void *x)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);
	if(px->loader) {
		px->loader->waitForThread(true);
		delete px->loader;
	}
	delete (pofJSON*)(((PdObject*)x)->parent);
}

void pofjson_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);

	if((argc>1) && argv->a_type == A_SYMBOL)
		outlet_anything(px->m_out1, atom_getsymbol(argv), argc-1, argv+1);
}

void pofjson_load(void *x, t_symbol *file)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);
	t_symbol* filename = makefilename(file,px->pdcanvas);
	
	if(px->loader) {
		px->loader->waitForThread(true);
		delete px->loader;
	}
	(px->loader = new JSONLoader(file, filename, px))->startThread();
}

void pofjson_getsize(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);	
	ofxJSONElement json = px->sjson->get(argc, argv);
	
	if(!json.empty()) outlet_float(px->m_out1, json.size());
}

void pofjson_getf(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);
	ofxJSONElement json = px->sjson->get(argc, argv);
	
	if((!json.empty()) && json.isConvertibleTo(Json::realValue))
		outlet_float(px->m_out1, json.asFloat());
}

void pofjson_gets(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofJSON* px= (pofJSON*)(((PdObject*)x)->parent);
	ofxJSONElement json = px->sjson->get(argc, argv);

	
	if(!json.empty()) {
		if(json.isConvertibleTo(Json::stringValue))	{
			t_symbol *s = gensym(json.asString().c_str());
			outlet_symbol(px->m_out1, gensym(json.asString().c_str()));
		}
		else outlet_symbol(px->m_out1, gensym(json.getRawString().c_str()));
	}
}

void pofJSON::setup(void)
{
	//post("pofjson_setup");
	
	s_out = gensym("out");
	s_errload = gensym("error_loading");
	s_loaded = gensym("loaded");
	
	pofjson_class = class_new(gensym("pofjson"), (t_newmethod)pofjson_new, (t_method)pofjson_free,
		sizeof(PdObject), 0, A_SYMBOL, A_NULL);
	class_addmethod(pofjson_class, (t_method)pofjson_out, s_out, A_GIMME, 0);
	class_addmethod(pofjson_class, (t_method)pofjson_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(pofjson_class, (t_method)pofjson_getf, gensym("getf"), A_GIMME, A_NULL);
	class_addmethod(pofjson_class, (t_method)pofjson_gets, gensym("gets"), A_GIMME, A_NULL);
	class_addmethod(pofjson_class, (t_method)pofjson_getsize, gensym("getsize"), A_GIMME, A_NULL);
}


