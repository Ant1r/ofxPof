/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofXML.h"

std::map<t_symbol*,pofsubXML*> pofsubXML::xmls;

static t_class *pofxml_class;
static t_symbol *s_set, *s_add, *s_setattr, *s_addchild, *s_remove, *s_removeattr, *s_get, *s_gets;

bool noreset = false;

static void *pofxml_new(t_symbol *n)
{
    pofXML* obj = new pofXML(pofxml_class, n);
    
    obj->pdcanvas = canvas_getcurrent();
    
    return (void*) (obj->pdobj);
}

static void pofxml_free(void *x)
{
	delete (pofXML*)(((PdObject*)x)->parent);
}

static void pofxml_load(void *x, t_symbol *file)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	t_atom at;
	t_symbol* filename = makefilename(file,px->pdcanvas);
	
	SETSYMBOL(&at, file);
	
	if(filename&&px->sxml->load(filename)) outlet_anything(px->m_out1, gensym("loaded"), 1, &at);
	else outlet_anything(px->m_out1, gensym("error_loading"), 1, &at);
}

static void pofxml_save(void *x, t_symbol *file)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	t_atom at;
	char buf[MAXPDSTRING];
	
	canvas_makefilename(px->pdcanvas, file->s_name, buf, MAXPDSTRING);
	sys_bashfilename(buf, buf);
	t_symbol* filename = gensym(buf);
	
	SETSYMBOL(&at, filename);
	
	if(filename&&px->sxml->xml.save(filename->s_name)) outlet_anything(px->m_out1, gensym("saved"), 1, &at);
	else outlet_anything(px->m_out1, gensym("error_saving"), 1, &at);
}

static void pofxml_setxml(void *x, t_symbol *name)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);

	pofsubXML::letXML(px->sxml);
	px->sxml = pofsubXML::getXML(name);
}

static void pofxml_get(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	
	string s = px->sxml->xml.getValue(path->s_name);
	if(s.length()) {
		binbuf_text(bb, (char*)s.c_str(), s.length());

		int natom = binbuf_getnatom(bb);
		if(natom) outlet_anything(px->m_out1, s_get, natom, binbuf_getvec(bb));
	}
	binbuf_free(bb);
}

static void pofxml_gets(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	t_atom at;

	string s = px->sxml->xml.getValue(path->s_name);
	if(s.length()) {
		SETSYMBOL(&at, gensym(s.c_str()));
		outlet_anything(px->m_out1, s_gets, 1, &at);
	}
}

static void pofxml_getnum(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	float num = 0;
	
	if(noreset) {
		num = px->sxml->xml.getNumChildren(path->s_name);
	} else {
		if(px->sxml->xml.exists(path->s_name) && px->sxml->xml.setTo(path->s_name)) num = px->sxml->xml.getNumChildren();
		px->sxml->xml.reset();
	}

	outlet_float(px->m_out1, num);
}

static void pofxml_remove(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	px->sxml->xml.remove(path->s_name);
}

static void pofxml_removeattr(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	
	if(noreset) {
		px->sxml->xml.removeAttributes(path->s_name);
	} else {
		if(px->sxml->xml.exists(path->s_name) && px->sxml->xml.setTo(path->s_name))
			px->sxml->xml.removeAttributes();
		px->sxml->xml.reset();
	}
}

static void pofxml_set(void *x, t_symbol *s,int argc, t_atom *argv)
{
	if(argc < 2 || argv->a_type != A_SYMBOL) return;
	
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	char *buf;
	int buflen;
	t_symbol * attr = NULL;
	
	t_symbol * path = atom_getsymbol(argv);
	argc--; argv++;

 	if(!argc) return;
 	
	t_binbuf *bb = binbuf_new();
	t_symbol * name = atom_getsymbol(argv);
	argc--; argv++;

 	if((s == s_addchild || s == s_setattr) && argc > 1) {
 		attr = atom_getsymbol(argv);
 		argc--; argv++;
 	}
 	
 	/*if(s == s_setattr) {
 		if(argc < 2 || argv->a_type != A_SYMBOL) return;
 		attr = atom_getsymbol(argv);
 		argc--; argv++;
 	}*/
 	
 	binbuf_add(bb, argc, argv);
	binbuf_gettext(bb, &buf, &buflen);

	string str("");
	char tbuf[30];
	while(argc > 0) {
		switch(argv->a_type){
			case A_FLOAT:
				sprintf(tbuf, "%g", argv->a_w.w_float);
				str += tbuf;//to_string(argv->a_w.w_float); 
				break;
    		case A_SYMBOL: 
    			str += argv->a_w.w_symbol->s_name; 
    			break;
    		default:;
    		
		}
		argc--; argv++;
		if(argc) str += " ";
	}
	
	if(noreset) {
		string totalpath;
		if(string(path->s_name) != ".") {
			totalpath = string(path->s_name);
			if(totalpath.back() != '/') totalpath += "/";
		}
		totalpath += string(name->s_name);
		if(s == s_set) px->sxml->xml.setValue(totalpath, str);
		else if(s == s_add) px->sxml->xml.addValue(totalpath, str, false);
		else if(s == s_setattr) {
			totalpath += "/" + string(attr->s_name);
			px->sxml->xml.setAttribute(totalpath, str);
		}
		else if(s == s_addchild) {
			if(px->sxml->xml.addChild(totalpath) && attr != NULL) {
				totalpath += "/" + string(attr->s_name);
				px->sxml->xml.setAttribute(totalpath, str);
			}
		}
	} else {
		if(px->sxml->xml.exists(path->s_name) && px->sxml->xml.setTo(path->s_name)) {
			if(s == s_set) px->sxml->xml.setValue(name->s_name, str);
			else if(s == s_add) px->sxml->xml.addValue(name->s_name, str, false);
			else if(s == s_setattr) {
				if(px->sxml->xml.setTo(name->s_name))
					px->sxml->xml.setAttribute(attr->s_name, str);

			}
			else if(s == s_addchild) {
				if(px->sxml->xml.addChild(name->s_name) && attr != NULL) {
					px->sxml->xml.setAttribute(attr->s_name, str);
				}
			}
		}
		px->sxml->xml.reset();
	}
	binbuf_free(bb);
	freebytes(buf, buflen);
}

static void pofxml_clear(void *x, t_symbol *root)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	px->sxml->xml.clear();
	if (!root || !*root->s_name) root = gensym("root");
	px->sxml->xml.addChild(root->s_name);
}

static void pofxml_print(void *x)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	
	string s = px->sxml->xml.toString();
	if(s.length()) post("xml %s: \n%s\n", px->name->s_name, s.c_str());
}

static void pofxml_noreset(void *x, t_float _noreset)
{
	noreset = (_noreset != 0);
}

static void pofxml_setto(void *x, t_symbol *path)
{
	pofXML* px= (pofXML*)(((PdObject*)x)->parent);
	if (path || path->s_name) px->sxml->xml.setTo(path->s_name);
	else px->sxml->xml.reset();
}


void pofXML::setup(void)
{
	//post("pofxml_setup");
	s_set = gensym("set");
	s_add = gensym("add");
	s_remove = gensym("remove");
	s_removeattr = gensym("removeattr");
	s_get = gensym("get");
	s_gets = gensym("gets");
	s_setattr = gensym("setattr");
	s_addchild = gensym("addchild");
	
	pofxml_class = class_new(gensym("pofxml"), (t_newmethod)pofxml_new, (t_method)pofxml_free,
		sizeof(PdObject), 0, A_SYMBOL, A_NULL);

	class_addmethod(pofxml_class, (t_method)pofxml_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_setxml, gensym("setxml"), A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_save, gensym("save"), A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_get, s_get, A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_gets, s_gets, A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_set, s_set, A_GIMME, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_set, s_add, A_GIMME, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_set, s_setattr, A_GIMME, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_set, s_addchild, A_GIMME, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_remove, s_remove, A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_getnum, gensym("getnum"), A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_removeattr, s_removeattr, A_SYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_clear, gensym("clear"), A_DEFSYMBOL, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_print, gensym("print"), A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_noreset, gensym("noreset"), A_DEFFLOAT, A_NULL);
	class_addmethod(pofxml_class, (t_method)pofxml_setto, gensym("setto"), A_SYMBOL, A_NULL);
}


