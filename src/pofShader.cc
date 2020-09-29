/*
 * Copyright (c) 2017 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofShader.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


t_class *pofShader_class;
static t_symbol *s_load, *s_play, *s_f, *s_2f, *s_3f, *s_4f, *s_tex, *s_print;

void *pofShader_new(t_symbol *s, int argc, t_atom *argv)
{	
    pofShader* obj = new pofShader(pofShader_class);
    obj->pdcanvas = canvas_getcurrent();
    return (void*) (obj->pdobj);
}

void pofShader_free(void *x)
{
	delete (pofShader*)(((PdObject*)x)->parent);
}

void pofShader_load(void *x, t_symbol *s, int argc, t_atom *argv)
{
	t_atom at[2];
	pofShader* px= (pofShader*)(((PdObject*)x)->parent);
	std::string str;
	
	if((argc == 1) && (argv->a_type == A_SYMBOL)) {
		str = string(atom_getsymbol(argv)->s_name) + ".vert";
		SETSYMBOL(&at[0], makefilename(gensym(str.c_str()), px->pdcanvas));
		str = string(atom_getsymbol(argv)->s_name) + ".frag";
		SETSYMBOL(&at[1], makefilename(gensym(str.c_str()), px->pdcanvas));
		pofBase::tellGUI(x, s_load, 2, at);
	} else if((argc == 2) && (argv->a_type == A_SYMBOL) && ((argv + 1)->a_type == A_SYMBOL)) {
		SETSYMBOL(&at[0], makefilename(atom_getsymbol(argv), px->pdcanvas));
		SETSYMBOL(&at[1], makefilename(atom_getsymbol(argv + 1), px->pdcanvas));
		pofBase::tellGUI(x, s_load, 2, at);
	}	
}

void pofShader::setup(void)
{
	s_load = gensym("load");
	s_play = gensym("play");
	s_f = gensym("f");
	s_2f = gensym("2f");
	s_3f = gensym("3f");
	s_4f = gensym("4f");
	s_tex = gensym("tex");
	s_print = gensym("print");
	
	pofShader_class = class_new(gensym("pofshader"), (t_newmethod)pofShader_new, (t_method)pofShader_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)pofShader_load, s_load, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_f, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_2f, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_3f, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_4f, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_tex, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_play, A_GIMME, A_NULL);
	class_addmethod(pofShader_class, (t_method)tellGUI, s_print, A_GIMME, A_NULL);

	POF_SETUP(pofShader_class);
}

void pofShader::draw()
{
	if(doShader) {
		shader.begin();
		sendUniforms();
	}
}

void pofShader::postdraw()
{
	if(doShader) shader.end();
}

void pofShader::getUniforms(){
	GLuint program = shader.getProgram();
	GLint numUniforms = 0;
	shaderUniform uniform;
	
	uniforms.clear();
	
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

	GLint uniformMaxLength = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength);
	
	GLint count = -1;
	GLenum type = 0;
	GLchar* uniformName = new GLchar[uniformMaxLength];
	for(GLint i = 0; i < numUniforms; i++) {
		GLsizei length;
		glGetActiveUniform(program, i, uniformMaxLength, &length, &count, &type, uniformName);
		uniform.type = type;
		uniform.used = FALSE;
		uniforms[uniformName] = uniform;
	}
}
/*
	https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGetActiveUniform.xml
	GL_FLOAT,
	GL_FLOAT_VEC2,
	GL_FLOAT_VEC3,
	GL_FLOAT_VEC4,
	GL_INT,
	GL_INT_VEC2,
	GL_INT_VEC3,
	GL_INT_VEC4,
	GL_BOOL,
	GL_BOOL_VEC2,
	GL_BOOL_VEC3,
	GL_BOOL_VEC4,
	GL_FLOAT_MAT2,
	GL_FLOAT_MAT3,
	GL_FLOAT_MAT4,
	GL_SAMPLER_2D,
	GL_SAMPLER_CUBE
*/
void pofShader::sendUniforms(){
	for (std::map<string,shaderUniform>::iterator it=uniforms.begin(); it!=uniforms.end(); ++it) {
		if(!it->second.used) continue;
		
		if(it->second.type == GL_FLOAT || it->second.type == GL_BOOL) {
			shader.setUniform1f(it->first, it->second.f[0]);
		} else if(it->second.type == GL_FLOAT_VEC2) {
			shader.setUniform2f(it->first, it->second.f[0], it->second.f[1]);
		} else if(it->second.type == GL_FLOAT_VEC3) {
			shader.setUniform3f(it->first, it->second.f[0], it->second.f[1], it->second.f[2]);
		} else if(it->second.type == GL_FLOAT_VEC4) {
			shader.setUniform4f(it->first, it->second.f[0], it->second.f[1], it->second.f[2], it->second.f[3]);
		} else if(it->second.type == GL_SAMPLER_2D 
#ifndef TARGET_OPENGLES
			|| it->second.type == GL_SAMPLER_2D_RECT
#endif
			) {
			ofTexture *tex = pofBase::textures[it->second.sym];
			if(tex) shader.setUniformTexture(it->first, *tex, (int)it->second.f[0]);
		}
	}
}

void pofShader::printUniforms(){
	std::cout << "pofShader::printUniforms():" << endl;
	for (std::map<string,shaderUniform>::iterator it=uniforms.begin(); it!=uniforms.end(); ++it) {
		if(it->second.used) std::cout << "*";
		else std::cout << " ";
		std::cout << it->first << " => "<< it->second.type << " : {"
			<< it->second.f[0] << "; " << it->second.f[1] << "; " << it->second.f[2] << "; " << it->second.f[3] << "}";
		if(it->second.sym && it->second.sym->s_name) std::cout << " sym = \"" << it->second.sym->s_name << "\"";
		std::cout << endl;
	}
}

void pofShader::message(int argc, t_atom *argv)
{
	t_atom ap[4];

	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;

	if(key == s_load) {
		if((argc == 1) && (argv->a_type == A_SYMBOL)) {
			if(shader.load(atom_getsymbol(argv)->s_name))
				getUniforms();
		} else if((argc == 2) && (argv->a_type == A_SYMBOL) && ((argv+1)->a_type == A_SYMBOL)) {
			if(shader.load(atom_getsymbol(argv)->s_name, atom_getsymbol(argv + 1)->s_name))
				getUniforms();
		}
	}
	else if(key == s_play) {
		if(argc < 1) return;
		doShader = (atom_getfloat(argv) != 0);
	}
	else if(key == s_f) {
		if((argc < 2) || (argv->a_type != A_SYMBOL)) return;
		if(uniforms[atom_getsymbol(argv)->s_name].type == GL_FLOAT 
		|| uniforms[atom_getsymbol(argv)->s_name].type == GL_BOOL) {
			uniforms[atom_getsymbol(argv)->s_name].f[0] = atom_getfloat(argv + 1);
			uniforms[atom_getsymbol(argv)->s_name].used = TRUE;
		}
	}
	else if(key == s_2f) {
		if((argc < 3) || (argv->a_type != A_SYMBOL)) return;
		if(uniforms[atom_getsymbol(argv)->s_name].type == GL_FLOAT_VEC2) {
			uniforms[atom_getsymbol(argv)->s_name].f[0] = atom_getfloat(argv + 1);
			uniforms[atom_getsymbol(argv)->s_name].f[1] = atom_getfloat(argv + 2);
			uniforms[atom_getsymbol(argv)->s_name].used = TRUE;
		}
	}
	else if(key == s_3f) {
		if((argc < 4) || (argv->a_type != A_SYMBOL)) return;
		if(uniforms[atom_getsymbol(argv)->s_name].type == GL_FLOAT_VEC3) {
			uniforms[atom_getsymbol(argv)->s_name].f[0] = atom_getfloat(argv + 1);
			uniforms[atom_getsymbol(argv)->s_name].f[1] = atom_getfloat(argv + 2);
			uniforms[atom_getsymbol(argv)->s_name].f[2] = atom_getfloat(argv + 3);
			uniforms[atom_getsymbol(argv)->s_name].used = TRUE;
		}
	}
	else if(key == s_4f) {
		if((argc < 5) || (argv->a_type != A_SYMBOL)) return;
		if(uniforms[atom_getsymbol(argv)->s_name].type == GL_FLOAT_VEC4) {
			uniforms[atom_getsymbol(argv)->s_name].f[0] = atom_getfloat(argv + 1);
			uniforms[atom_getsymbol(argv)->s_name].f[1] = atom_getfloat(argv + 2);
			uniforms[atom_getsymbol(argv)->s_name].f[2] = atom_getfloat(argv + 3);
			uniforms[atom_getsymbol(argv)->s_name].f[3] = atom_getfloat(argv + 4);
			uniforms[atom_getsymbol(argv)->s_name].used = TRUE;
		}
	}
	else if(key == s_tex) {
		if((argc < 3) || (argv->a_type != A_SYMBOL) || ((argv + 1)->a_type != A_SYMBOL)) return;
		//std::cout<<"set texture:"<<atom_getsymbol(argv)->s_name<<"->"<<atom_getsymbol(argv + 1)->s_name<<","<<atom_getfloat(argv + 2)<<endl;
		if(uniforms[atom_getsymbol(argv)->s_name].type == GL_SAMPLER_2D
#ifndef TARGET_OPENGLES
		|| uniforms[atom_getsymbol(argv)->s_name].type == GL_SAMPLER_2D_RECT
#endif
		) {
			//std::cout<<"set texture ok"<<endl;
			uniforms[atom_getsymbol(argv)->s_name].sym = atom_getsymbol(argv + 1); // symbolic texture name
			uniforms[atom_getsymbol(argv)->s_name].f[0] = atom_getfloat(argv + 2); // texture location
			uniforms[atom_getsymbol(argv)->s_name].used = TRUE;
		}		
	}
	else if(key == s_print) {
		printUniforms();
	}
}

