/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofXMLP.h"

std::map<t_symbol*,pofsubXMLP*> pofsubXMLP::xmls;

static t_class *pofxmlp_class;
static t_symbol *s_set, *s_add, *s_setattr, *s_addchild, *s_remove, *s_removeattr, *s_get, *s_gets;

static void *pofxmlp_new(t_symbol *n)
{
    pofXMLP* obj = new pofXMLP(pofxmlp_class, n);
    
    obj->pdcanvas = canvas_getcurrent();
    
    return (void*) (obj->pdobj);
}

static void pofxmlp_free(void *x)
{
	delete (pofXMLP*)(((PdObject*)x)->parent);
}

static void pofxmlp_load(void *x, t_symbol *file)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_atom at;
	t_symbol* filename = makefilename(file,px->pdcanvas);
	
	SETSYMBOL(&at, file);
	
	if(filename&&px->sxml->load(filename)) outlet_anything(px->m_out1, gensym("loaded"), 1, &at);
	else outlet_anything(px->m_out1, gensym("error_loading"), 1, &at);
}

static void pofxmlp_save(void *x, t_symbol *file)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_atom at;
	char buf[MAXPDSTRING];
	
	canvas_makefilename(px->pdcanvas, file->s_name, buf, MAXPDSTRING);
	sys_bashfilename(buf, buf);
	t_symbol* filename = gensym(buf);
	
	SETSYMBOL(&at, filename);
	
	if(filename&&px->sxml->save(filename)) outlet_anything(px->m_out1, gensym("saved"), 1, &at);
	else outlet_anything(px->m_out1, gensym("error_saving"), 1, &at);
}

static void pofxmlp_setxml(void *x, t_symbol *name)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);

	pofsubXMLP::letXML(px->sxml);
	px->sxml = pofsubXMLP::getXML(name);
}

static void pofxmlp_get(void *x, t_symbol *path)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	string s;
	try{
		pugi::xml_node node = px->sxml->doc.root().select_node(path->s_name).node();
		s = node.text().as_string();
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
	
	if(s.length()) {
		binbuf_text(bb, (char*)s.c_str(), s.length());

		int natom = binbuf_getnatom(bb);
		if(natom) outlet_anything(px->m_out1, s_get, natom, binbuf_getvec(bb));
	}
	binbuf_free(bb);
}

static void pofxmlp_gets(void *x, t_symbol *path)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_atom at;

	string s = px->sxml->doc.root().select_node(path->s_name).node().text().as_string();
	string s2 = px->sxml->doc.root().select_node(path->s_name).node().value();
	cout << "value: " << s2 << endl;
	if(s.length()) {
		SETSYMBOL(&at, gensym(s.c_str()));
		outlet_anything(px->m_out1, s_gets, 1, &at);
	}
}

static void pofxmlp_getnum(void *x, t_symbol *path)
{
	/*pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	float num = 0;
	
	if(px->sxml->xml.exists(path->s_name) && px->sxml->xml.setTo(path->s_name)) num = px->sxml->xml.getNumChildren();
	px->sxml->xml.reset();

	outlet_float(px->m_out1, num);*/
}

static void pofxmlp_remove(void *x, t_symbol *path)
{
	/*pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	px->sxml->xml.remove(path->s_name);*/
}

static void pofxmlp_removeattr(void *x, t_symbol *path)
{
	/*pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	if(px->sxml->xml.exists(path->s_name) && px->sxml->xml.setTo(path->s_name))
		px->sxml->xml.removeAttributes();//(path->s_name);
	px->sxml->xml.reset();*/
}

static void pofxmlp_set(void *x, t_symbol *s,int argc, t_atom *argv)
{
#if 0	
	if(argc < 2 || argv->a_type != A_SYMBOL) return;
	
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	char *buf;
	int buflen;
	t_symbol * attr = NULL;
	
	t_symbol * path = atom_getsymbol(argv);
	argc--; argv++;

 	if(!argc) return;
 	
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
	binbuf_free(bb);
	freebytes(buf, buflen);
#endif
}

static void pofxmlp_clear(void *x, t_symbol *root)
{
	/*pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	px->sxml->xml.clear();
	if (!root || !*root->s_name) root = gensym("root");
	px->sxml->xml.addChild(root->s_name);*/
}

static void pofxmlp_print(void *x)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	std::stringstream strstr;
	px->sxml->doc.print(strstr);
	std::string line; 
	post("XML %s:", px->name->s_name);
	while (std::getline(strstr, line)) {
		post("%s", line.c_str());
	}
}


void pofXMLP::setup(void)
{
	//post("pofxmlp_setup");
	s_set = gensym("set");
	s_add = gensym("add");
	s_remove = gensym("remove");
	s_removeattr = gensym("removeattr");
	s_get = gensym("get");
	s_gets = gensym("gets");
	s_setattr = gensym("setattr");
	s_addchild = gensym("addchild");
	
	pofxmlp_class = class_new(gensym("pofxmlp"), (t_newmethod)pofxmlp_new, (t_method)pofxmlp_free,
		sizeof(PdObject), 0, A_SYMBOL, A_NULL);

	class_addmethod(pofxmlp_class, (t_method)pofxmlp_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_setxml, gensym("setxml"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_save, gensym("save"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_get, s_get, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_gets, s_gets, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_set, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_add, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_setattr, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_addchild, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_remove, s_remove, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_getnum, gensym("getnum"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_removeattr, s_removeattr, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_clear, gensym("clear"), A_DEFSYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_print, gensym("print"), A_NULL);
}


