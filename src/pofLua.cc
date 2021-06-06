/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofLua.h"
#include "ofxLua.h"

t_class *pofLua_class;

ofxLua lua;

void *pofLua_new(t_symbol *s, int argc, t_atom *argv)
{
	pofLua* obj = new pofLua(pofLua_class);
	t_symbol *name = obj->s_self;

	if(argc && argv->a_type == A_SYMBOL) {
		name = atom_getsymbol(argv);
		argv++; argc--;
	}

	if(*name->s_name == 0 || name->s_thing != 0) name = obj->s_self;
	obj->name = name;
	if(obj->name != obj->s_self) pd_bind(&obj->pdobj->x_obj.ob_pd, name);

	while(argc && *atom_getsymbol(argv)->s_name != ';') { argv++; argc--; }
	if(argc && *atom_getsymbol(argv)->s_name == ';') { argv++; argc--; }

	std::ostringstream ss;
	string namestr = string(name->s_name);
	/*ss << "package.preload['" << namestr << "'] = nil package.loaded['" << namestr << "'] = nil\n"
	<< "package.preload['" << namestr << "'] = function(this) local M = {}\n";*/
	ss << namestr << " = {}; local M = " << namestr <<"; ";

	for (int i = 0; i < argc; ++i)
	{
		if (argv[i].a_type == A_SYMBOL)
			ss << argv[i].a_w.w_symbol->s_name;
		else if (argv[i].a_type == A_FLOAT)
			ss << argv[i].a_w.w_float;
		ss << ' ';
	}
	//ss << "\nreturn M end";

	cout << ss.str() << endl;
	if(!lua.doString(ss.str())) {
		pd_error(obj, "pofLua: %s", lua.getErrorMessage().c_str());
		//error("pofLua: %s", lua.getErrorMessage().c_str());
	}
	return (void*) (obj->pdobj);
}

void pofLua_free(void *x)
{
	pofLua* obj = (pofLua*)(((PdObject*)x)->parent);
	if(obj->name != obj->s_self) pd_unbind(&obj->pdobj->x_obj.ob_pd, obj->name);
	delete obj;
}


void pofLua::setup(void)
{
	//post("pofLua_setup");
	pofLua_class = class_new(gensym("poflua"), (t_newmethod)pofLua_new, (t_method)pofLua_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofLua_class);
	// init the lua state
	lua.init();
}

pofLua::pofLua(t_class *Class): pofBase(Class)
{
}
pofLua::~pofLua()
{
}


void pofLua::draw()
{
	ofPushMatrix();
	ofPushStyle();
	//lua.scriptDraw();
	//cout << "local M=require(" + string(name->s_name) + "); M.draw();" << endl;
	//if(!lua.doString("local M=require('" + string(name->s_name) + "'); M.draw();"))
	if(!lua.doString(string(name->s_name) + ".draw();"))
	{
		cout << lua.getErrorMessage() << endl;
	}
}

void pofLua::postdraw()
{
	ofPopMatrix();
	ofPopStyle();
}

