/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofPath.h"

t_class *pofpath_class;
static t_symbol *s_clear, *s_close, *s_move, *s_line, *s_curve, *s_arc; 

#define NEXT_FLOAT_ARG(var) if((argc>0) && (argv->a_type == A_FLOAT)) { var = atom_getfloat(argv); argv++; argc--; }

void pofpath_scale(void *x, float width, float height);

void *pofpath_new(t_symbol *s, int argc, t_atom *argv)
{
    pofPath* obj = new pofPath(pofpath_class);

    float w=0, h=0;

    NEXT_FLOAT_ARG(w);
    NEXT_FLOAT_ARG(h);
    pofpath_scale(obj->pdobj, w, h);
    
    return (void*) (obj->pdobj);
}

void pofpath_free(void *x)
{
	delete (pofPath*)(((PdObject*)x)->parent);
}

void pofpath_fill(void *x, float r, float g, float b, float a)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.setFillColor(ofColor(r*255.0, g*255.0, b*255.0, a*255.0));
}

void pofpath_stroke(void *x, float r, float g, float b, float a)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.setStrokeColor(ofColor(r*255.0, g*255.0, b*255.0, a*255.0));
}

void pofpath_width(void *x, float width)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.setStrokeWidth(width);
}

void pofpath_filled(void *x, float filled)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.setFilled(filled!=0);
}

void pofpath_res(void *x, float res)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.setCurveResolution(res);
	px->path.setCircleResolution(res);
}

void pofpath_mesh(void *x, float mesh)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->doMesh = (mesh != 0);
}

void pofpath_scale(void *x, float width, float height)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->scale.set(width, height);
}

void pofpath_tellGui(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->queueToGUI(s, argc, argv);
}

void pofPath::setup(void)
{
	//post("pofpath_setup");
	s_clear = gensym("clear");
	s_close = gensym("close");
	s_move = gensym("moveTo");
	s_line = gensym("lineTo");
	s_curve = gensym("curveTo");
	s_arc = gensym("arc");
	
	pofpath_class = class_new(gensym("pofpath"), (t_newmethod)pofpath_new, (t_method)pofpath_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	
	class_addmethod(pofpath_class, (t_method)tellGUI, s_clear,  A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)tellGUI, s_close,  A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)tellGUI, s_move,   A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)tellGUI, s_line,   A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)tellGUI, s_curve,  A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)tellGUI, s_arc,    A_GIMME, A_NULL);
	
	class_addmethod(pofpath_class, (t_method)pofpath_fill, gensym("fillColor"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_stroke, gensym("strokeColor"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_width, gensym("strokeWidth"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_filled, gensym("filled"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_res, gensym("res"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_mesh, gensym("mesh"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_scale, gensym("scale"), A_DEFFLOAT, A_DEFFLOAT, A_NULL);

	POF_SETUP(pofpath_class);
}

void pofPath::draw()
{
	if(doMesh && (currentTexture != NULL)) {
		ofMesh mesh = path.getTessellation();
		ofPoint textureSize(currentTexture->getWidth(), currentTexture->getHeight());
		if(scale.x && scale.y) for(auto & v: mesh.getVertices()){
			mesh.addTexCoord(currentTexture->getCoordFromPoint(textureSize.x/scale.x*v.x + 
					textureSize.x/2 ,textureSize.y/scale.y*v.y + textureSize.y/2 ) );
		}
		else for(auto & v: mesh.getVertices()){
			mesh.addTexCoord(currentTexture->getCoordFromPoint(v.x + textureSize.x/2 ,v.y + textureSize.y/2 ) /*- textureSize/2*/ );
		}
		mesh.draw();
	}
	else path.draw();
}

void pofPath::message(int argc, t_atom *argv)
{
	float X=0, Y=0, Z=0, radiusX=0, radiusY=0, angleBegin=0, angleEnd=0;	

	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;
  
	if(key == s_clear) path.clear();
	else if(key == s_close) path.close();
	else if(key == s_move) {
		NEXT_FLOAT_ARG(X);
		NEXT_FLOAT_ARG(Y);
		NEXT_FLOAT_ARG(Z);
		path.moveTo(X, Y, Z);
	}
	else if(key == s_line) {
		NEXT_FLOAT_ARG(X);
		NEXT_FLOAT_ARG(Y);
		NEXT_FLOAT_ARG(Z);
		path.lineTo(X, Y, Z);
	}
	else if(key == s_curve) {
		NEXT_FLOAT_ARG(X);
		NEXT_FLOAT_ARG(Y);
		NEXT_FLOAT_ARG(Z);
		path.curveTo(X, Y, Z);
	}
	else if(key == s_arc) {
		NEXT_FLOAT_ARG(X);
		NEXT_FLOAT_ARG(Y);
		NEXT_FLOAT_ARG(Z);
		NEXT_FLOAT_ARG(radiusX);
		NEXT_FLOAT_ARG(radiusY);
		NEXT_FLOAT_ARG(angleBegin);
		NEXT_FLOAT_ARG(angleEnd);
		if(angleBegin > angleEnd) {
			float tmp = angleEnd;
			angleEnd = angleBegin;
			angleBegin = tmp;
		}
		path.arc(X, Y, Z, radiusX, radiusY, angleBegin, angleEnd);
	}
}  

