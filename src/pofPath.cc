/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofPath.h"

t_class *pofpath_class;

#define NEXT_FLOAT_ARG(var) if(argc>0) { if(argv->a_type == A_FLOAT) var = atom_getfloat(argv); argv++; argc--; }

void *pofpath_new(t_symbol *s)
{
    pofPath* obj = new pofPath(pofpath_class);
    
    return (void*) (obj->pdobj);
}

void pofpath_free(void *x)
{
	delete (pofPath*)(((PdObject*)x)->parent);
}

void pofpath_clear(void *x)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.clear();
}

void pofpath_close(void *x)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.close();
}

void pofpath_move(void *x, float X, float Y, float Z)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.moveTo(X, Y, Z);
}

void pofpath_line(void *x, float X, float Y, float Z)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.lineTo(X, Y, Z);
}

void pofpath_curve(void *x, float X, float Y, float Z)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	px->path.curveTo(X, Y, Z);
}

void pofpath_arc(void *x, t_symbol *s, int argc, t_atom *argv)
{
	pofPath* px = (pofPath*)(((PdObject*)x)->parent);
	
	float X=0, Y=0, Z=0, radiusX=0, radiusY=0, angleBegin=0, angleEnd=0;
	
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
	
	px->path.arc(X, Y, Z, radiusX, radiusY, angleBegin, angleEnd);
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

void pofPath::setup(void)
{
	//post("pofpath_setup");
	pofpath_class = class_new(gensym("pofpath"), (t_newmethod)pofpath_new, (t_method)pofpath_free,
		sizeof(PdObject), 0, A_NULL);

	class_addmethod(pofpath_class, (t_method)pofpath_clear, gensym("clear"), A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_close, gensym("close"), A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_move, gensym("moveTo"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_line, gensym("lineTo"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_curve, gensym("curveTo"), A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_arc, gensym("arc"), A_GIMME, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_fill, gensym("fillColor"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_stroke, gensym("strokeColor"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_width, gensym("strokeWidth"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_filled, gensym("filled"), A_FLOAT, A_NULL);
	class_addmethod(pofpath_class, (t_method)pofpath_res, gensym("res"), A_FLOAT, A_NULL);

	POF_SETUP(pofpath_class);
}

void pofPath::draw()
{
	path.draw();
}

