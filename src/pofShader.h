/*
 * Copyright (c) 2017 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

typedef struct _shaderUniform {
	GLuint type;
	bool used;
	float f[4];
	t_symbol *sym;
} shaderUniform; 

class pofShader: public pofBase {
	public:
		pofShader(t_class *Class):pofBase(Class), doShader(true){  }

		virtual void draw();
		virtual void postdraw(); // called after objects bellow have been drawn
		virtual void message(int  arc, t_atom *argv);
		
		void getUniforms();
		void sendUniforms();
		void printUniforms();
		
		static void setup(void);
		
		ofShader shader;
		bool doShader;
		std::map<string,shaderUniform> uniforms;
		t_canvas *pdcanvas;
};



