/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofLua.h"
#include "ofxLua.h"
#include "pofFonts.h"
#include "pofFbo.h"

t_class *pofLua_class, *pofLua_receiver_class;

static ofxLua lua;
static ofMutex luaMutex;
static t_symbol *s_out, *s_function, *s_method, *s_global, *s_receive;
static std::map<string, pofLua*> pofLuas;

// ------------ pofLua_receiver -------------

pofLua_receiver::pofLua_receiver() : pd(NULL)
{}

pofLua_receiver::~pofLua_receiver()
{
	if(pd) pd_unbind(&pd, name);
}

void pofLua_receiver::initialize(pofLua *owner, t_symbol *_name)
{
	pd = pofLua_receiver_class;
	lua = owner;
	name = _name;
	pd_bind(&pd, name);
}

void pofLua_receiver::rcv_anything(t_symbol *s, int argc, t_atom *argv)
{
	t_atom at;
	t_binbuf *bb = binbuf_new();
	SETSYMBOL(&at, s_receive);
	binbuf_add(bb, 1, &at);
	SETSYMBOL(&at, name);
	binbuf_add(bb, 1, &at);
	if(s != &s_list && s != &s_symbol && s != &s_float) {
		SETSYMBOL(&at, s);
		binbuf_add(bb, 1, &at);
	}
	binbuf_add(bb, argc, argv);
	lua->queueToGUI(s_method, binbuf_getnatom(bb), binbuf_getvec(bb));
	lua->trigger = true;
	binbuf_free(bb);
}

static void pofLua_receiver_anything(pofLua_receiver *x, t_symbol *s, int argc, t_atom *argv)
{
	x->rcv_anything(s, argc, argv);
}

void pofLua_receiver::setup()
{
	pofLua_receiver_class = class_new(gensym("pofluarcv"), 0, 0, sizeof(pofLua_receiver), CLASS_PD, A_NULL);
	class_addanything(pofLua_receiver_class, pofLua_receiver_anything);
}

// ------------ Methods exported to Lua -------------

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
		else if(type == LUA_TBOOLEAN) vec.push_back((float)(lua_toboolean(L, i)?1.0:0.0));
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

static void pofLua_lua_drawconfig(lua_State *L)
{
	if(lua_type (L, 1) != LUA_TSTRING) return;
	if(lua_type (L, 2) != LUA_TSTRING) return;
	string objname = lua_tostring(L, 1);
	pofLua *obj = pofLuas[objname];
	if(!obj) return;

	string command = lua_tostring(L, 2);
	if(command == "do") {
		obj->trigger = true;
	} else if(command == "force") {
		obj->force = true;
	} else if(command == "continuousForce") {
		obj->continuousForce = lua_toboolean(L, 3);
	}
}

static int pofLua_lua_getfile(lua_State *L)
{
	if(lua_type (L, 1) != LUA_TSTRING) return 0;
	if(lua_type (L, 2) != LUA_TSTRING) return 0;

	string objname = string(lua_tostring(L, 1));
	const char* filename = lua_tostring(L, 2);
	pofLua *obj = pofLuas[objname];
	if(!obj) return 0;

	int fd;
	char namebuf[MAXPDSTRING+1], *namebufptr;

	if ((fd = canvas_open(obj->pdcanvas, filename, "", namebuf, &namebufptr, MAXPDSTRING, 1)) < 0)
	{
		pd_error(obj->pdobj, "pofLua_lua_getfile: can't open %s", filename);
		return 0;
	}
	string absfilename = string(namebuf) + "/" + string(namebufptr);
	lua_pushstring(lua, absfilename.c_str());
	return 1;
}

static t_symbol *getsym(string symaddr)
{
	stringstream ss(symaddr);
	long long unsigned int i;
	ss >> std::hex >> i;
	return reinterpret_cast<t_symbol *>(i);
}

ofTexture *textures_get(string texsymaddr) {
	return pofBase::textures[getsym(texsymaddr)];
}

ofxFontStash *fonts_get(string fontsymaddr) {
	pofFonts *pfs = pofFonts::getFont(getsym(fontsymaddr));
	if(pfs) return pfs->offont;
	else return NULL;
}

ofFbo *fbo_get(string fbosymaddr) {
	auto it = pofsubFbo::sfbos.find(getsym(fbosymaddr));
	if(it != pofsubFbo::sfbos.end() && it->second->fbo->isAllocated()) return it->second->fbo;
	else return NULL;
}

// ------------ module creation and script loading utilities -------------

static string pofLua_prefix(void *x)
{
	pofLua* obj = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	std::ostringstream ss;
	string namestr = string(obj->name->s_name);
	string commandstr =
		namestr + " = " + namestr + " or {} local M = " + namestr + ";" +
		"M.pdself = '" + namestr + "' ;" +
		"function M.out(...) topd(M.pdself, 'out', ...) end; "
		"function M.send(...) topd(M.pdself, 'send', ...) end; "
		"function M.touchconfig(...) topd(M.pdself, 'touchconfig', ...) end; "
		"function M.drawconfig(...) drawconfig(M.pdself, ...) end; "
		"function M.addreceive(name) topd(M.pdself, 'receive', name) end; "
		"function M.getfile(...) return getfile(M.pdself, ...) end; "
	;

	return commandstr;
}

static string dollarsymbol(string instring)
{
	string outstring;
	std::size_t index = instring.find("$\"");
	std::size_t lastindex = 0;
	
	while(index != std::string::npos) {
		outstring.append(instring.substr(lastindex, index - lastindex));
		lastindex = index;
		size_t nextindex = instring.find('"', index + 2);
		if(nextindex != std::string::npos) {
			string tmpstr = instring.substr(index + 2, nextindex - index - 2);
			t_symbol *s = gensym(tmpstr.c_str());
			stringstream ss;
			ss << s;
			//cout << "dollarsymbol: " << tmpstr.c_str() << " -> " << ss.str() << endl;
			outstring.append("'" + ss.str() + "'");
			index = lastindex = nextindex + 1;
		}
		index = instring.find("$\"", index + 1);
	}
	outstring.append(instring.substr(lastindex, index - lastindex));
	//cout << "final string: " << outstring << endl;
	return outstring;
}

static void pofLua_reload(void *x)
{
	pofLua* obj = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	int fd;
	char namebuf[MAXPDSTRING], *namebufptr;
	long length;
	int readret;
	char *buf;

	obj->script = pofLua_prefix(x);

	if(obj->filename) {
		if ((fd = canvas_open(obj->pdcanvas, obj->filename->s_name, "", namebuf, &namebufptr, MAXPDSTRING, 0)) < 0)
		{
			pd_error(x, "pofLua_read: can't open %s", obj->filename->s_name);
			return;
		}

		if ((length = (long)lseek(fd, 0, SEEK_END)) < 0 || lseek(fd, 0, SEEK_SET) < 0
			|| !(buf = (char *)t_getbytes(length + 1)))
		{
			pd_error(x, "pofLua_read %s lseek: %s", obj->filename->s_name, strerror(errno));
			close(fd);
			return;
		}
	
		if ((readret = (int)read(fd, buf, length)) < length)
		{
			pd_error(x, "pofLua_read %s read: %s", obj->filename->s_name, strerror(errno));
			close(fd);
			t_freebytes(buf, length);
			return;
		}
		close (fd);
		buf[length] = 0;
		obj->script += string(buf);
		t_freebytes(buf, length);
	}
	
	obj->receivers.clear();
	obj->script += obj->argsScript;
	obj->script = dollarsymbol(obj->script);
	obj->loaded = obj->touchable = obj->drawable = false;
	obj->trigger = true;
}

// ------------ pd class methods -------------

static void *pofLua_new(t_symbol *s, int argc, t_atom *argv)
{
	pofLua* obj = new pofLua(pofLua_class);
	t_symbol *name = obj->s_self;

	obj->pdcanvas = canvas_getcurrent();
	obj->filename = NULL;
	
	if(argc && argv->a_type == A_SYMBOL && *atom_getsymbol(argv)->s_name != ';') {
		name = atom_getsymbol(argv);
		argv++; argc--;
	}

	if(*name->s_name == 0 || name->s_thing != 0) name = obj->s_self;
	obj->name = name;
	if(obj->name != obj->s_self) pd_bind(&obj->pdobj->x_obj.ob_pd, name);

	while(argc && *atom_getsymbol(argv)->s_name != ';') { 
		if(atom_getsymbol(argv) == gensym("-r")) {
			argv++; argc--;
			if(argc) {
				obj->filename = atom_getsymbol(argv);
				argv++; argc--;
			}
		}
		else {argv++; argc--;}
	}

	if(argc && *atom_getsymbol(argv)->s_name == ';') { argv++; argc--; }

	std::ostringstream ss;
	ss << pofLua_prefix(obj->pdobj);
	for (int i = 0; i < argc; ++i)
	{
		if (argv[i].a_type == A_SYMBOL)
			ss << argv[i].a_w.w_symbol->s_name;
		else if (argv[i].a_type == A_FLOAT)
			ss << argv[i].a_w.w_float;
		ss << ' ';
	}

	obj->argsScript = ss.str();
	
	pofLuas[obj->name->s_name] = obj;

	pofLua_reload(obj->pdobj);
	return (void*) (obj->pdobj);
}

static void pofLua_free(void *x)
{
	pofLua* obj = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	if(obj->name != obj->s_self) pd_unbind(&obj->pdobj->x_obj.ob_pd, obj->name);
	delete obj;
	pofLuas.erase(obj->name->s_name);
}

static void pofLua_lua(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* obj = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	t_binbuf *bb = binbuf_new();
	char *buf;
	int bufsize;
	binbuf_add(bb, argc, argv);
	binbuf_gettext(bb, &buf, &bufsize);
	binbuf_free(bb);
	string str = "local M=" + string(obj->name->s_name) + ";" + buf;
	luaMutex.lock();
	if(!lua.doString(str.c_str())) {
		pd_error(x, "pofLua: %s", lua.getErrorMessage().c_str());
		//error("pofLua: %s", lua.getErrorMessage().c_str());
	}
	luaMutex.unlock();
	t_freebytes(buf, bufsize);
}

static void pofLua_lua_async(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	pofBase::tellGUI(x, s, argc, argv);
	px->trigger = true;
}

static void pofLua_out(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	if(!argc) return;
	if(argv->a_type == A_SYMBOL) outlet_anything(px->m_out2, atom_getsymbol(argv), argc - 1, argv + 1);
	else outlet_list(px->m_out2, s, argc, argv);
}

static void pofLua_send(void *x, t_symbol *s, int argc, t_atom *argv)
{
	(void) x;
	if(!argc) return;
	if(argv->a_type != A_SYMBOL) return;
	t_symbol *dest = atom_getsymbol(argv);
	if (!dest->s_thing) return;
	argc--; argv++;
	if(argv->a_type == A_SYMBOL) typedmess(dest->s_thing, atom_getsymbol(argv), argc - 1, argv + 1);
	else pd_list(dest->s_thing, gensym("list"), argc, argv);
}

static void pofLua_receive(void *x, t_symbol *s)
{
	pofLua* px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	px->receivers[s].initialize(px, s);
}

static void pofLua_touchconfig(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofLua* px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	t_symbol *command;
	if(!argc) return;
	if(argv->a_type == A_SYMBOL) {
		command = atom_getsymbol(argv);
		argv++; argc--;
		if(command == gensym("size") && argc > 1) {
			px->width = atom_getfloat(argv);
			px->height = atom_getfloat(argv + 1);
		}
		else if(command == gensym("dont_capture") && argc > 0) {
			float dc = atom_getfloat(argv);
			px->capture = (dc==0) || (dc==3);
			px->dynamic = (dc>=2);
		}
		else if(command == gensym("multi") && argc > 0) {
			float m = atom_getfloat(argv);
			px->multi = (m != 0);
		}
	}
}

static void pofLua_bang(void *x)
{
	pofLua *px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	px->trigger = true;
}

static void pofLua_continuousForce(void *x, t_float t)
{
	pofLua *px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	px->continuousForce = (t != 0);
}

static void pofLua_force(void *x)
{
	pofLua *px = dynamic_cast<pofLua*>(((PdObject*)x)->parent);
	px->force = true;
}

extern "C" {
	int luaopen_pof(lua_State* L);
}

void pofLua::setup(void)
{
	s_out = gensym("out");
	s_function = gensym("f");
	s_method = gensym("m");
	s_global = gensym("g");
	s_receive = gensym("receive");

	pofLua_receiver::setup();

	pofLua_class = class_new(gensym("poflua"), (t_newmethod)pofLua_new, (t_method)pofLua_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	POF_SETUP(pofLua_class);
	class_addmethod(pofLua_class, (t_method)pofLua_lua, gensym("lua"), A_GIMME, A_NULL);

	class_addmethod(pofLua_class, (t_method)pofLua_lua_async, s_function, A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_lua_async, s_method, A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_lua_async, s_global, A_GIMME, A_NULL);

	class_addmethod(pofLua_class, (t_method)pofLua_out, s_out, A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_send, gensym("send"), A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_receive, gensym("receive"), A_SYMBOL, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_touchconfig, gensym("touchconfig"), A_GIMME, A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_reload, gensym("reload"), A_NULL);
	class_addbang(pofLua_class, (t_method)pofLua_bang);
	class_addmethod(pofLua_class, (t_method)pofLua_force, gensym("force"), A_NULL);
	class_addmethod(pofLua_class, (t_method)pofLua_continuousForce, gensym("continuousForce"), A_DEFFLOAT, A_NULL);

	// init the lua state
	lua.init();
	luaopen_pof(lua);
	lua_pushcfunction(lua, (lua_CFunction)pofLua_print);
	lua_setglobal(lua, "print");
	lua_pushcfunction(lua, (lua_CFunction)pofLua_lua_topd);
	lua_setglobal(lua, "topd");
	lua_pushcfunction(lua, (lua_CFunction)pofLua_lua_drawconfig);
	lua_setglobal(lua, "drawconfig");
	lua_pushcfunction(lua, (lua_CFunction)pofLua_lua_getfile);
	lua_setglobal(lua, "getfile");
	lua_pushboolean(lua, false);
	lua_setglobal(lua, "FORCE_DRAW");
}


// ------------ C++ class methods -------------

pofLua::pofLua(t_class *Class):
	pofBase(Class), pofTouch(Class, 200, 200), pofOnce(Class, true),
	loaded(false), touchable(false), drawable(false)
{
}

pofLua::~pofLua()
{
}

void pofLua::Send(t_symbol *s, int n, float f1, float f2, float f3)
{
	luaMutex.lock();
	
	if(lua.pushTable(name->s_name)) {
		if(lua.isFunction("touch")) {
			lua_getfield(lua, -1, "touch");
			lua_getglobal(lua, name->s_name);
			lua_pushstring(lua, s->s_name);
			lua_pushnumber(lua, f1);
			lua_pushnumber(lua, f2);
			lua_pushnumber(lua, f3);
			if(lua_pcall(lua, 5, 0, 0) != 0) {
				pd_error(pdobj, "Error running touch(): %s", lua_tostring(lua, -1));
			}
		}
		lua.popTable();
	}
	else pd_error(pdobj, "pofLua::Send pushTable %s: %s", name->s_name, lua.getErrorMessage().c_str());
	
	luaMutex.unlock();
}

void pofLua::draw()
{
	if(!loaded) {
		luaMutex.lock();
		if(!lua.doString(script)) {
			pd_error(pdobj, "pofLua: %s", lua.getErrorMessage().c_str());
		}
		else {
			if(lua.pushTable(name->s_name)) {
				touchable = lua.isFunction("touch");
				drawable = lua.isFunction("draw");
				lua.popTable();
			}
			else pd_error(pdobj, "pofLua loading pushTable %s: %s", name->s_name, lua.getErrorMessage().c_str());
		}
		luaMutex.unlock();
		pofBase::needBuild = true; // needed to rebuild the touchtree
		loaded = true;
	}
	if(!drawable) return;
	ofPushMatrix();
	ofPushStyle();
	luaMutex.lock();
	
	lua_pushboolean(lua, FORCE_ONCE);
	lua_setglobal(lua, "FORCE_DRAW");

	if(lua.pushTable(name->s_name)) {
		lua_getfield(lua, -1, "draw");
		lua_getglobal(lua, name->s_name);
		if(lua_pcall(lua, 1, 0, 0) != 0) {
			pd_error(pdobj, "Error running draw(): %s", lua_tostring(lua, -1));
		}
		lua.popTable();
	}
	else pd_error(pdobj, "pofLua drawing pushTable %s: %s", name->s_name, lua.getErrorMessage().c_str());
	luaMutex.unlock();
}

void pofLua::postdraw()
{
	if(drawable) {
		ofPopMatrix();
		ofPopStyle();
	}
}

void pofLua::message(int argc, t_atom *argv)
{
	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;

	if(!loaded) return;
	luaMutex.lock();
	if(key == s_function || key == s_method || key == s_global) {
		t_symbol *func = atom_getsymbol(argv);
		int n = 0;
		argv++; argc--;
		if(key == s_function || key == s_method) {
			if(!lua.pushTable(name->s_name)) {
				pd_error(pdobj, "pofLua::message %s: %s", name->s_name, lua.getErrorMessage().c_str());
				goto end;
			}
		}
		if(lua.isFunction(func->s_name)) {
			if(key == s_function || key == s_method) {
				lua_getfield(lua, -1, func->s_name);
				if(key == s_method) {
					lua_getglobal(lua, name->s_name);
					n = 1;
				}
			} else {
				lua_getglobal(lua, func->s_name);
			}
			while(argc) {
				if(argv->a_type == A_SYMBOL) lua_pushstring(lua, atom_getsymbol(argv)->s_name);
				else if(argv->a_type == A_FLOAT) lua_pushnumber(lua, atom_getfloat(argv));
				argv++; argc--; n++;
			}
			if(lua_pcall(lua, n, 0, 0) != 0) {
				pd_error(pdobj, "pofLua::message calling %s(): %s", func->s_name, lua_tostring(lua, -1));
			}
		}
		if(key == s_function || key == s_method) lua.popTable();
	}
end:
	luaMutex.unlock();
}
