/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofScope.h"

t_class *pofscope_class;

void *pofscope_new(t_floatarg w,t_floatarg h, t_float len)
{
	if(w <= 0) w = 1;
	if(len <= 0) len = 1;

	pofScope* obj = new pofScope(pofscope_class, w, h, len);

	floatinlet_new(&obj->pdobj->x_obj, &obj->width);
	floatinlet_new(&obj->pdobj->x_obj, &obj->height);

	obj->bufLen = len;
	if(obj->bufLen < w) obj->bufLen = int(w);

	obj->bufCount = 0;
	obj->bufIndex = 0;

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

void pofscope_compute(void *x, float comp, float once)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);

	px->compute = (comp != 0);
	px->once = (once != 0);
	if(px->once) {
		px->bufCount = 0;
		//post("once");
	}
}

void pofscope_peaks(void *x, t_symbol *peakstab, float from, float length)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);

	t_garray *peaksArray;
	int peaksLen;
	t_word *vec = NULL;

	if (!(peaksArray = (t_garray *)pd_findbyclass(peakstab, garray_class)))
		pd_error(x, "%s: no such array", peakstab->s_name);
	else if (!garray_getfloatwords(peaksArray, &peaksLen, &vec))
		pd_error(x, "%s: bad template for tabdump", peakstab->s_name);
	if(!vec) return;

	if(from < 0) from = 0;
	if(from >= peaksLen) from = peaksLen - 1;
	if(length == 0) length = 1e10;
	if((length + from) >= peaksLen) length = peaksLen - from - 1;
	if(length < 0) length = 0;

	px->Mutex.lock();
	px->peaksVec = vec;
	px->peaksFrom = from;
	px->peaksLen = length;
	px->compute = false;
	px->readPeaks = true;
	px->Mutex.unlock();
}

static t_int *pofscope_perform(t_int *w)
{
	pofScope* px = (pofScope*)(((PdObject*)w[1])->parent);
	t_sample *in = (t_sample *)(w[2]);
	t_int n = *(t_int *)(w+3);

	if (px->compute) {
		px->Mutex.lock();
		int W = px->curWidth;
		while (n--)
		{
			if (!px->compute) break;
			t_sample f = *in++;
			int i = ((float)px->bufCount / px->bufLen) * W;
			int oldi = i;

			px->bufIndex = i + 1;
			if(f > px->maxBuf[i]) px->maxBuf[i % W] = f;
			if(f < px->minBuf[i]) px->minBuf[i % W] = f;

			px->bufCount++;
			if(px->bufCount >= px->bufLen) {
				if(px->once) px->compute = 0;
				px->bufCount = px->bufCount % px->bufLen;
			}

			i = ((float)px->bufCount / px->bufLen) * W;
			if(oldi != i) px->maxBuf[i % W] = px->minBuf[i % W] = f;
		}
		px->Mutex.unlock();
	}

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
	class_addmethod(pofscope_class, (t_method)pofscope_compute, gensym("compute"), A_FLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_peaks, gensym("peaks"), A_SYMBOL,
		A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	CLASS_MAINSIGNALIN(pofscope_class, PdObject, x_f);
	class_addmethod(pofscope_class, (t_method)pofscope_dsp, gensym("dsp"), A_NULL);
}

static void decodePeak(float data, float *min, float *max)
{
	*min = (((int)data)%2048 - 1024) / 1024.0;
	*max = (((int)data)/2048 - 1024) / 1024.0;
}

void pofScope::draw()
{
	int j;
	bool doReadPeaks;
	t_word *pVec;
	float pFrom;
	float pLen;

	if(curWidth != int(width)) {
		Mutex.lock();
		delete [] minBuf;
		delete [] maxBuf;
		if(width < 1) width = 1;
		curWidth = int(width);
		minBuf = new float[curWidth];
		maxBuf = new float[curWidth];
		std::fill_n(minBuf, curWidth, 0);
		std::fill_n(maxBuf, curWidth, 0);
		bufIndex = 0;
		Mutex.unlock();
	}

	Mutex.lock();
	if((doReadPeaks = readPeaks)) {
		pVec = peaksVec;
		pFrom = peaksFrom;
		pLen = peaksLen;
		readPeaks = false;
	}
	Mutex.unlock();

	if(doReadPeaks) {
		int i, j;
		if(curWidth < pLen) {
			float min, max;
			int oldj = -1;
			for(i = 0; i < pLen ; ++i) {
				j = (i * curWidth) / pLen;
				if(j != oldj) {
					decodePeak(pVec[i + (int)pFrom].w_float, &minBuf[j], &maxBuf[j]);
					oldj = j;
				}
				decodePeak(pVec[i + (int)pFrom].w_float, &min, &max);
				if(min < minBuf[j]) minBuf[j] = min;
				if(max > maxBuf[j]) maxBuf[j] = max;
				bufIndex = 0;
			}
		} else {
			float frac, ratio = pLen / (float)curWidth;
			float min1 = 0, max1 = 0, min2 = 0, max2 = 0;
			int oldi = -1;
			for(j = 0; j < curWidth ; ++j) {
				i = (int)((float)j * ratio + pFrom);
				if(i != oldi) {
					oldi = i;
					decodePeak(pVec[i].w_float, &min1, &max1);
					decodePeak(pVec[i + 1].w_float, &min2, &max2);
				}
				frac = ((float)j * ratio  + pFrom) - i;
				minBuf[j] = (1 - frac) * min1 + frac * min2;
				maxBuf[j] = (1 - frac) * max1 + frac * max2;
			}
		}
	}

	for(int i = 0; i < curWidth ; ++i) {
		j = (int(i + bufIndex /*+ 1*/))%curWidth;
		ofRect(i - curWidth/2, minBuf[j] * height, 1, (maxBuf[j] - minBuf[j]) * height + 1);
	}
}
