/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofLua.h"
#include "ofxLua.h"

static t_class *pofLua_class;

static ofxLua lua;
static ofMutex luaMutex;
static t_symbol *s_out;

static void *pofLua_new(t_symbol *s, int argc, t_atom *argv)
{
	pofLua* obj = new pofLua(pofLua_class);
	t_symbol *name = obj->s_self;

	if(argc && argv->a_type == A_SYMBOL && *atom_getsymbol(argv)->s_name != ';') {
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
	ss << namestr << " = {} local M = " << namestr <<" ";
	ss << "M.pdself = '" << namestr << "' ";
	ss << "function M.out(...) topd(M.pdself, 'out', ...) end; ";
	ss << "function M.config(...) topd(M.pdself, 'config', ...) end; ";

	for (int i = 0; i < argc; ++i)
	{
		if (argv[i].a_type == A_SYMBOL)
			ss << argv[i].a_w.w_symbol->s_name;
		else if (argv[i].a_type == A_FLOAT)
			ss << argv[i].a_w.w_float;
		ss << ' ';
	}

	//cout << ss.str() << endl;
	
	luaMutex.lock();
	if(!lua.doString(ss.str())) {
		pd_error(obj->pdobj, "pofLua: %s", lua.getErrorMessage().c_str());
		luaMutex.unlock();
		delete obj;
		return 0;
		//error("pofLua: %s", lua.getErrorMessage().c_str());
	}
	lua.pushTable(name->s_name);
	obj->touchable = lua.isFunction("touch");
	lua.popTable();

	luaMutex.unlock();
	return (void*) (obj->pdobj);
}

static void pofLua_free(void *x)
{
	pofLua* obj = (pofLua*)(((PdObject*)x)->parent);
	if(obj->name != obj->s_self) pd_unbind(&obj->pdobj->x_obj.ob_pd, obj->name);
	delete obj;
}

static void pofLua_lua(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* obj = (pofLua*)(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	char *buf;
	int bufsize;
	binbuf_add(bb, argc, argv);
	binbuf_gettext(bb, &buf, &bufsize);
	binbuf_free(bb);
	//post("lua: %s", buf);
	string str = "local M=" + string(obj->name->s_name) + ";" + buf;
	luaMutex.lock();
	if(!lua.doString(str.c_str())) {
		pd_error(x, "pofLua: %s", lua.getErrorMessage().c_str());
		//error("pofLua: %s", lua.getErrorMessage().c_str());
	}
	luaMutex.unlock();
	t_freebytes(buf, bufsize);
}

static void pofLua_stackToBinbuf(lua_State *L, int first, t_binbuf *bb)
{
	int top = lua_gettop(L);
	for(int i = first; i <= top; i++)
	{
		t_atom at;
		int type = lua_type (L, i);
		if(type == LUA_TNUMBER) SETFLOAT(&at, lua_tonumber(L, i));
		else if(type == LUA_TBOOLEAN) SETFLOAT(&at, lua_toboolean(L, i)?1:0);
		else if(type == LUA_TSTRING) SETSYMBOL(&at, gensym(lua_tostring(L, i)));
		else continue;
		binbuf_add(bb, 1, &at);
	}
}

static std::vector<Any> pofLua_stackToVec(lua_State *L, int first)
{
	int top = lua_gettop(L);
	std::vector<Any> vec;
	for(int i = first; i <= top; i++)
	{
		int type = lua_type (L, i);
		if(type == LUA_TNUMBER) vec.push_back((float)lua_tonumber(L, i));
		else if(type == LUA_TBOOLEAN) vec.push_back((float)lua_toboolean(L, i)?1.0:0.0);
		else if(type == LUA_TSTRING) vec.push_back(string(lua_tostring(L, i)));
		else continue;
	}
	return vec;
}

static void pofLua_print(lua_State *L)
{
	t_binbuf *bb = binbuf_new();
	pofLua_stackToBinbuf(L, 1, bb);
	binbuf_print(bb);
	binbuf_free(bb);
}

static void pofLua_lua_topd(lua_State *L)
{
	int type = lua_type (L, 1);
	if(type != LUA_TSTRING) return;

	std::vector<Any> vec = pofLua_stackToVec(L, 1);
	pofBase::sendToPd(vec);
}

static void pofLua_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* px = (pofLua*)(((PdObject*)x)->parent);
	if(!argc) return;
	if(argv->a_type == A_SYMBOL) outlet_anything(px->m_out2, atom_getsymbol(argv), argc - 1, argv + 1);
	else outlet_list(px->m_out2, s, argc, argv);
}

static void pofLua_config(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* px = (pofLua*)(((PdObject*)x)->parent);
	if(!argc) return;
	if(argv->a_type == A_SYMBOL) outlet_anything(px->m_out2, atom_getsymbol(argv), argc - 1, argv + 1);
	else outlet_list(px->m_out2, s, argc, argv);
}

void pofLua::setup(void)
{
	s_out = gensym("out");
	//post("pofLua_setup");
	pofLua_class = class_new(gensym("poflua"), (t_newmethod)pofLua_new, (t_method)pofLua_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofLua_class);
	class_addmethod(pofLua_class, (t_method)pofLua_lua, gensym("lua"),	A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_out, s_out, A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_config, gensym("config"), A_GIMME, A_NULL);

	//class_addanything(pofLua_class, (t_method)tellGUI, s_anything,	A_GIMME, A_NULL);
	// init the lua state
	lua.init();
	lua_pushcfunction(lua, (lua_CFunction)pofLua_print);
	lua_setglobal(lua, "print");
	lua_pushcfunction(lua, (lua_CFunction)pofLua_lua_topd);
	lua_setglobal(lua, "topd");
}

pofLua::pofLua(t_class *Class): pofTouch(Class, 200, 200)
{
}
pofLua::~pofLua()
{
}

void pofLua::Send(t_symbol *s, int n, float f1, float f2, float f3)
{
	//pofTouch::Send(s, n, f1, f2, f3);
	luaMutex.lock();
	lua.pushTable(name->s_name);
	if(lua.isFunction("touch")) {
		lua_getfield(lua, -1, "touch");
		lua_pushstring(lua, s->s_name);
		lua_pushnumber(lua, f1);
		lua_pushnumber(lua, f2);
		lua_pushnumber(lua, f3);
		if(lua_pcall(lua, 4, 0, 0) != 0) {
			pd_error(pdobj, "Error running touch(): %s", lua_tostring(lua, -1));
		}
	}
	lua.popTable();
	luaMutex.unlock();
}

void pofLua::draw()
{
	ofPushMatrix();
	ofPushStyle();
	//lua.scriptDraw();
	//cout << "local M=require(" + string(name->s_name) + "); M.draw();" << endl;
	//if(!lua.doString("local M=require('" + string(name->s_name) + "'); M.draw();"))
	/*if(!lua.doString(string(name->s_name) + ".draw();"))
	{
		cout << lua.getErrorMessage() << endl;
	}*/
	luaMutex.lock();
	lua.pushTable(name->s_name);
	if(lua.isFunction("draw")) {
		lua_getfield(lua, -1, "draw");
		if(lua_pcall(lua, 0, 0, 0) != 0) {
			pd_error(pdobj, "Error running draw(): %s", lua_tostring(lua, -1));
		}
	}
	lua.popTable();
	luaMutex.unlock();
}

void pofLua::postdraw()
{
	ofPopMatrix();
	ofPopStyle();
}

