/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */

// see: https://sites.google.com/site/ofauckland/examples/quad-warping

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
	px->needUpdate = true;
}

void pofquad_corner(void *x, float num, float X, float Y, float Z)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	if((num >= 0) && (num <= 3)) px->corners[int(num)].set(X, Y, Z);
	px->needUpdate = true;
}

void pofquad_tcorner(void *x, float num, float U, float V)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	if((num >= 0) && (num <= 3)) px->tcorners[int(num)].set(U, V);
	px->needUpdate = true;
}

void pofquad_drawMesh(void *x, float on)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	px->drawMesh = (on!=0);
}

void pofquad_drawVertices(void *x, float on)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	px->drawVertices = (on!=0);
}

void pofquad_drawWireframe(void *x, float on)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	px->drawWireframe = (on!=0);
}

void pofquad_drawFaces(void *x, float on)
{
	pofQuad* px = (pofQuad*)(((PdObject*)x)->parent);
	
	px->drawFaces = (on!=0);
}

void pofQuad::setup(void)
{
	//post("pofquad_setup");
	pofquad_class = class_new(gensym("pofquad"), (t_newmethod)pofquad_new, (t_method)pofquad_free,
		sizeof(PdObject), 0, A_GIMME, A_NULL);
	class_addmethod(pofquad_class, (t_method)pofquad_res, gensym("res"), A_FLOAT, A_FLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_corner, gensym("corner"), A_FLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_tcorner, gensym("tcorner"), A_FLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_drawMesh, gensym("drawMesh"), A_FLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_drawVertices, gensym("drawVertices"), A_FLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_drawWireframe, gensym("drawWireframe"), A_FLOAT, A_NULL );
	class_addmethod(pofquad_class, (t_method)pofquad_drawFaces, gensym("drawFaces"), A_FLOAT, A_NULL );

	POF_SETUP(pofquad_class);
}


static ofPoint ofxLerp(ofPoint start, ofPoint end, float amt) {
    return start + amt * (end - start);
}

static int ofxIndex(float x, float y, float w) {
    return y*w+x;
}

void pofQuad::Update() {
	ofPoint lt = corners[0], rt = corners[1], rb = corners[2], lb = corners[3];
#ifndef TARGET_OPENGLES
	float tw = textureSize.x;
	float th = textureSize.y;
#else 
	float tw = 1;
	float th = 1;
#endif
	ofPoint tsize(tw, th);
	ofPoint tlt = tcorners[0] * tsize, trt = tcorners[1] * tsize, trb = tcorners[2] * tsize, tlb = tcorners[3] * tsize;
	
	mesh.clear();
	for (int x = 0; x <= columns; x++) {
		float f = float(x) / columns;
		ofPoint vTop(ofxLerp(lt, rt, f));
		ofPoint vBottom(ofxLerp(lb, rb, f));
		ofPoint tTop(ofxLerp(tlt, trt, f));
		ofPoint tBottom(ofxLerp(tlb, trb, f));
		//ofPoint tTop(ofxLerp(ofPoint(0,0),ofPoint(tw,0),f));
		//ofPoint tBottom(ofxLerp(ofPoint(0,th),ofPoint(tw,th),f));
        
		for (int y=0; y<=rows; y++) {
			float f = float(y)/rows;
			ofPoint v = ofxLerp(vTop,vBottom,f);
			mesh.addVertex(v);
			mesh.addTexCoord(ofxLerp(tTop,tBottom,f));
		}
    }
    
	for (float y=0; y<rows; y++) {
		for (float x=0; x<columns; x++) {
			mesh.addTriangle(ofxIndex(x,y,columns+1), ofxIndex(x+1,y,columns+1), ofxIndex(x,y+1,columns+1));
			mesh.addTriangle(ofxIndex(x+1,y,columns+1), ofxIndex(x+1,y+1,columns+1), ofxIndex(x,y+1,columns+1));
		}
	}
    
	needUpdate = false;
}

void pofQuad::draw() {
    //if(!pofBase::currentTexture) return;
    ofPoint newTexureSize = textureSize;
	if(currentTexture != NULL) {
		newTexureSize = ofPoint(currentTexture->getWidth(), currentTexture->getHeight());
		if(newTexureSize != textureSize) needUpdate = true;
		textureSize = newTexureSize;
	}
	
	if(needUpdate) Update();
	
    if (drawMesh) mesh.draw();
    if (drawVertices) mesh.drawVertices();
    if (drawWireframe) mesh.drawWireframe();
    if (drawFaces) mesh.drawFaces();
}


