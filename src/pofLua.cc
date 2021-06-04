/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofLua.h"
#include "ofxLua.h"

t_class *pofLua_class;

ofxLua lua;

void *pofLua_new()
{
	pofLua* obj = new pofLua(pofLua_class);

	obj->script = 
		"function draw();"
			"of.fill();"
			"for i=0,200 do;"
				"of.setColor(of.random(0, 255), of.random(0, 255), of.random(0, 255), 255);"
				"of.drawRectangle(of.random(-100, 100), of.random(-100, 100), of.random(10, 20), of.random(10, 20));"
			"end;"
			"/*of.setHexColor(0xFFFFFFFF);*/"
			"/*of.drawBitmapString(\"rectangles\", 275, 500);*/"
		"end;"
	;
	string script2 = 
		"function draw();"
			"for i=0,200 do;"
				"of.setColor(of.random(0, 255), of.random(0, 255), of.random(0, 255), 255);"
				"of.drawRectangle(of.random(-100, 100), of.random(-100, 100), of.random(10, 20), of.random(10, 20));"
			"end;"
		"end;"
	;
	lua.doString(script2);
	return (void*) (obj->pdobj);
}

void pofLua_free(void *x)
{
	delete (pofLua*)(((PdObject*)x)->parent);
}


void pofLua::setup(void)
{
	//post("pofLua_setup");
	pofLua_class = class_new(gensym("poflua"), (t_newmethod)pofLua_new, (t_method)pofLua_free,
		sizeof(PdObject), 0, A_NULL);
	POF_SETUP(pofLua_class);
	// init the lua state
	lua.init();
}

void pofLua::draw()
{
	ofPushMatrix();
	ofPushStyle();
	lua.scriptDraw();
}

void pofLua::postdraw()
{
	ofPopMatrix();
	ofPopStyle();
}

