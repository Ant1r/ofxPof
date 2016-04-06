/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofQuad.h"

t_class *pofquad_class;

#define NEXT_FLOAT_ARG(var) if(argc>0) { if(argv->a_type == A_FLOAT) var = atom_getfloat(argv); argv++; argc--; }

void *pofquad_new(t_symbol *s, int argc, t_atom *argv)
{
/*    float w=0, h=0;

    NEXT_FLOAT_ARG(w);
    NEXT_FLOAT_ARG(h);*/
    
    pofQuad* obj = new pofQuad(pofquad_class);
    
    //floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    //floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    
    return (void*) (obj->pdobj);
}

void pofquad_free(void *x)
{
	delete (pofQuad*)(((PdObject*)x)->parent);
}

void pofquad_res(void *x, float resX, float resY)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	px->rows = resX;
	px->columns = resY;
}

void pofquad_corner(void *x, float num, float X, float Y, float Z)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	if((num>=0)&&(num<=3)) px->corners[int(num)].set(X,Y,Z);
}

void pofQuad::setup(void)
{
	//post("pofquad_setup");
	pofquad_class = class_new(gensym("pofquad"), (t_newmethod)pofquad_new, (t_method)pofquad_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofquad_class, (t_method)pofquad_res, gensym("res"), A_FLOAT, A_FLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_corner, gensym("corner"), A_FLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL );

	POF_SETUP(pofquad_class);
}


ofPoint ofxLerp(ofPoint start, ofPoint end, float amt) {
    return start + amt * (end - start);
}

int ofxIndex(float x, float y, float w) {
    return y*w+x;
}

void ofxQuadWarp(ofPoint lt, ofPoint rt, ofPoint rb, ofPoint lb, int rows, int cols) {
    if(!pofBase::currentTexture) return;
    
    float tw = 1;//pofBase::currentTexture->getWidth();
    float th = 1;//pofBase::currentTexture->getHeight();
    
    ofMesh mesh;
    
    for (int x=0; x<=cols; x++) {
        float f = float(x)/cols;
        ofPoint vTop(ofxLerp(lt,rt,f));
        ofPoint vBottom(ofxLerp(lb,rb,f));
        ofPoint tTop(ofxLerp(ofPoint(0,0),ofPoint(tw,0),f));
        ofPoint tBottom(ofxLerp(ofPoint(0,th),ofPoint(tw,th),f));
        
        for (int y=0; y<=rows; y++) {
            float f = float(y)/rows;
            ofPoint v = ofxLerp(vTop,vBottom,f);
            mesh.addVertex(v);
            mesh.addTexCoord(ofxLerp(tTop,tBottom,f));
        }
    }
    
    for (float y=0; y<rows; y++) {
        for (float x=0; x<cols; x++) {
            mesh.addTriangle(ofxIndex(x,y,cols+1), ofxIndex(x+1,y,cols+1), ofxIndex(x,y+1,cols+1));
            mesh.addTriangle(ofxIndex(x+1,y,cols+1), ofxIndex(x+1,y+1,cols+1), ofxIndex(x,y+1,cols+1));
        }
    }
    
    mesh.draw();
    //mesh.drawVertices();
}

void pofQuad::draw()
{
    ofxQuadWarp(corners[0], corners[1], corners[2], corners[3], rows, columns);
}


