/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofXMLP.h"

std::map<t_symbol*,pofsubXMLP*> pofsubXMLP::xmls;

static t_class *pofxmlp_class;
static t_symbol *s_set, *s_add, *s_setattr, *s_remove, *s_removeall, *s_removeattr, *s_get, *s_gets;

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

static void pofxmlp_setto(void *x, t_symbol *path)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	try{
		px->sxml->node = px->sxml->node.select_node(path->s_name).node();
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
}

static void pofxmlp_get(void *x, t_symbol *path, t_symbol *attr)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	string s;
	try{
		pugi::xml_node node = px->sxml->node.select_node(path->s_name).node();
		if(*attr->s_name) s = node.attribute(attr->s_name).as_string();
		else s = node.text().as_string();
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

static void pofxmlp_gets(void *x, t_symbol *path, t_symbol *attr)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	t_atom at;
	string s;
	t_binbuf *bb = binbuf_new();
	pugi::xpath_node_set node_set;
	try{
		for(auto n: px->sxml->node.select_nodes(path->s_name)) {
			if(*attr->s_name) s = n.node().attribute(attr->s_name).as_string();
			else s = n.node().text().as_string();
			if(s.length()) {
				SETSYMBOL(&at, gensym(s.c_str()));
				binbuf_add(bb, 1, &at);
			}
		}
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
	outlet_anything(px->m_out1, s_gets, binbuf_getnatom(bb), binbuf_getvec(bb));
	binbuf_free(bb);
}

static void pofxmlp_getnum(void *x, t_symbol *path)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	float num = 0;
	try{
		num = px->sxml->node.select_nodes(path->s_name).size();
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
	outlet_float(px->m_out1, num);
}

static void pofxmlp_remove(void *x, t_symbol *path, t_symbol *child)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	try{
		pugi::xml_node node;
		node = px->sxml->node.select_node(path->s_name).node();
		node.remove_child(child->s_name);
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
}

static void pofxmlp_removeall(void *x, t_symbol *path, t_symbol *child)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	try{
		for(auto n: px->sxml->node.select_nodes(path->s_name)) {
			n.node().parent().remove_child(n.node());
		}
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
}

static void pofxmlp_removeattr(void *x, t_symbol *path, t_symbol *attr)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	try{
		pugi::xml_node node;
		node = px->sxml->node.select_node(path->s_name).node();
		node.remove_attribute(attr->s_name);
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
}

static void pofxmlp_set(void *x, t_symbol *s,int argc, t_atom *argv)
{
	if(argc < 2 || argv->a_type != A_SYMBOL) return;
	
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	char *buf;
	int buflen;
	t_symbol * name = &s_;
	
	t_symbol * path = atom_getsymbol(argv);
	argc--; argv++;

 	if(!argc) return;
 	
 	if((s == s_add || s == s_setattr) && argc > 0) {
 		name = atom_getsymbol(argv);
 		argc--; argv++;
 	}
 	
	t_binbuf *bb = binbuf_new();
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
	
	pugi::xml_node node;
	try{
		node = px->sxml->node.select_node(path->s_name).node();
		if(s == s_set) node.text().set(str.c_str());
		else if(s == s_add) {
			pugi::xml_node newchild = node.append_child(name->s_name);
			if(str.length()) newchild.text().set(str.c_str());
		}
		else if(s == s_setattr) {
			node.remove_attribute(name->s_name);
			node.append_attribute(name->s_name).set_value(str.c_str());
		}
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
		return;
	}

	binbuf_free(bb);
	freebytes(buf, buflen);
}

static void pofxmlp_clear(void *x, t_symbol *root)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	px->sxml->doc.reset();
	if (!*root->s_name) root = gensym("root");
	px->sxml->doc.append_child(root->s_name);
}

static void pofxmlp_print(void *x, t_symbol *path)
{
	pofXMLP* px= (pofXMLP*)(((PdObject*)x)->parent);
	std::stringstream strstr;
	try{
		if(*path->s_name) px->sxml->node.select_node(path->s_name).node().print(strstr);
		else px->sxml->doc.print(strstr);
	}catch(pugi::xpath_exception & e){
		ofLogError() << e.what();
	}
	post("XML %s:", px->name->s_name);
	std::string line; 
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
	s_removeall = gensym("removeall");
	s_removeattr = gensym("removeattr");
	s_get = gensym("get");
	s_gets = gensym("gets");
	s_setattr = gensym("setattr");
	
	pofxmlp_class = class_new(gensym("pofxmlp"), (t_newmethod)pofxmlp_new, (t_method)pofxmlp_free,
		sizeof(PdObject), 0, A_SYMBOL, A_NULL);

	class_addmethod(pofxmlp_class, (t_method)pofxmlp_load, gensym("load"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_setxml, gensym("setxml"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_save, gensym("save"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_setto, gensym("setto"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_get, s_get, A_SYMBOL, A_DEFSYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_gets, s_gets, A_SYMBOL, A_DEFSYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_set, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_add, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_set, s_setattr, A_GIMME, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_remove, s_remove, A_SYMBOL, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_removeall, s_removeall, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_removeattr, s_removeattr, A_SYMBOL, A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_getnum, gensym("getnum"), A_SYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_clear, gensym("clear"), A_DEFSYMBOL, A_NULL);
	class_addmethod(pofxmlp_class, (t_method)pofxmlp_print, gensym("print"), A_DEFSYMBOL, A_NULL);
}


