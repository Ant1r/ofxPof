/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofScope.h"

t_class *pofscope_class;

void *pofscope_new(t_floatarg w,t_floatarg h, t_float len)
{
    pofScope* obj = new pofScope(pofscope_class, w, h, len);
    
    floatinlet_new(&obj->pdobj->x_obj, &obj->width);
    floatinlet_new(&obj->pdobj->x_obj, &obj->height);
    
    obj->bufLen = len;
    if(obj->bufLen < w) obj->bufLen = int(w);
    
    return (void*) (obj->pdobj);
}

void pofscope_free(void *x)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	delete px;
}

void pofscope_buflen(void *x, float len)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	
	px->bufLen = len;
	if(px->bufLen < px->width) px->bufLen = int(px->width);
	if(px->bufLen < 1) px->bufLen = 1;
}

void pofscope_compute(void *x, float comp)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	
	px->compute = (comp != 0);
}

static t_int *pofscope_perform(t_int *w)
{
    pofScope* px = (pofScope*)(((PdObject*)w[1])->parent);
    t_sample *in = (t_sample *)(w[2]);
    t_int n = *(t_int *)(w+3);
        
    //px->Mutex.lock();
    
    int W = px->curWidth;
    
    if (px->compute) while (n--)
    {   
        t_sample f = *in++;
        int i = int(px->bufIndex);
        int oldi = i;
        if(f > px->maxBuf[i]) px->maxBuf[i % W] = f;
        if(f < px->minBuf[i]) px->minBuf[i % W] = f;

        px->bufIndex += (((float)W) / px->bufLen);
        while(px->bufIndex > W) px->bufIndex -= W;

        i = int(px->bufIndex);
		if(oldi != i) px->maxBuf[i % W] = px->minBuf[i % W] = f;
	}
   
    //px->Mutex.unlock();
    
	return (w + 4);
}

static void pofscope_dsp(void *x, t_signal **sp)
{
    dsp_add(pofscope_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

void pofScope::setup(void)
{
	//post("pofscope_setup");
	pofscope_class = class_new(gensym("pofscope"), (t_newmethod)pofscope_new, (t_method)pofscope_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	POF_SETUP(pofscope_class);
	class_addmethod(pofscope_class, (t_method)pofscope_buflen, gensym("buflen"), A_FLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_compute, gensym("compute"), A_FLOAT, A_NULL);
    CLASS_MAINSIGNALIN(pofscope_class, PdObject, x_f);
    class_addmethod(pofscope_class, (t_method)pofscope_dsp, gensym("dsp"), A_NULL);

}

void pofScope::draw()
{
	int j;
	
    Mutex.lock();
	if(curWidth != int(width)) {
		delete [] minBuf;
		delete [] maxBuf;
		if(width < 1) width = 1;
		curWidth = int(width);
		minBuf = new float[curWidth];
		maxBuf = new float[curWidth];
		std::fill_n(minBuf, curWidth, 0);
		std::fill_n(maxBuf, curWidth, 0);
		bufIndex = 0;
	}
	
	for(int i = 0; i < curWidth ; ++i) {
		j = (int(i + bufIndex + 1))%curWidth;
		ofRect(i - curWidth/2, minBuf[j] * height, 1, (maxBuf[j] - minBuf[j]) * height + 1);
	}
	Mutex.unlock();

}
