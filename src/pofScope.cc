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

void pofscope_strokeColor(void *x, float r, float g, float b, float a)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->strokeColor = ofFloatColor(r, g, b, a);
}

void pofscope_fillColor(void *x, float r, float g, float b, float a)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->fillColor = ofFloatColor(r, g, b, a);
}

void pofscope_curve(void *x, float v)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->curve = v;
}

void pofscope_stroke(void *x, float v)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->stroke = v;
}

void pofscope_fill(void *x, float v)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->fill = v;
	px->updateGUI = true;
}

void pofscope_strokeWidth(void *x, float w)
{
	pofScope* px = (pofScope*)(((PdObject*)x)->parent);
	px->strokeWidth = w;
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
		px->updateGUI = true;
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
	class_addmethod(pofscope_class, (t_method)pofscope_curve, gensym("curve"),
		A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_stroke, gensym("stroke"),
		A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_fill, gensym("fill"),
		A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_strokeColor, gensym("strokeColor"),
		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_fillColor, gensym("fillColor"),
		A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofscope_class, (t_method)pofscope_strokeWidth, gensym("strokeWidth"),
		A_DEFFLOAT, A_NULL);

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
	t_word *pVec = NULL;
	float pFrom = 0;
	float pLen = 0;

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
		updateGUI = true;
		Mutex.unlock();
	}

	Mutex.lock();
	if((doReadPeaks = readPeaks)) {
		pVec = peaksVec;
		pFrom = peaksFrom;
		pLen = peaksLen;
		readPeaks = false;
		updateGUI = true;
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

	if(updateGUI && curve) {
		updateGUI = false;
		minpath.clear();
		maxpath.clear();
		fillpath.clear();
		minpath.setFilled(false);
		maxpath.setFilled(false);
		fillpath.setFilled(true);
		#define MINPOS(i) (i) - curWidth/2.0, minBuf[int((i) + bufIndex)%curWidth] * height
		#define MAXPOS(i) (curWidth/2.0 - 1 - (i)), maxBuf[int(curWidth - 1 - (i) + bufIndex)%curWidth] * height
        minpath.moveTo(MINPOS(0));
		maxpath.moveTo(ofPoint(MAXPOS(0)) + ofPoint(1,0));
        
        if(curve == 1) {
            for(int i = 0; i < curWidth ; ++i) {
                j = (int(i + bufIndex))%curWidth;
                minpath.curveTo(MINPOS(i));
                maxpath.curveTo(MAXPOS(i));
            }
            minpath.curveTo(ofPoint(MINPOS(curWidth - 1)) + ofPoint(1,0));
            minpath.curveTo(ofPoint(MINPOS(curWidth - 1)) + ofPoint(1,0));
        } else {
            for(int i = 0; i < curWidth ; ++i) {
                j = (int(i + bufIndex))%curWidth;
                minpath.lineTo(MINPOS(i));
                maxpath.lineTo(MAXPOS(i));
            }
            minpath.lineTo(ofPoint(MINPOS(curWidth - 1)) + ofPoint(1,0));
            minpath.lineTo(ofPoint(MINPOS(curWidth - 1)) + ofPoint(1,0));
        }
		maxpath.lineTo(MAXPOS(curWidth - 1.0));
		
		if(fill) {
			fillpath.append(minpath);
			fillpath.lineTo(ofPoint(MAXPOS(0)) + ofPoint(1,0));
			fillpath.append(maxpath);
			fillpath.lineTo(MINPOS(0));
			fillpath.close();
		}
	}

	if(curve == 0) {
		for(int i = 0; i < curWidth ; ++i) {
			j = (int(i + bufIndex))%curWidth;
			ofRect(i - curWidth/2.0, minBuf[j] * height, 1, (maxBuf[j] - minBuf[j]) * height + 1);
		}
	} else {
		ofColor styleColor = ofGetStyle().color;
		#define COLMULT(X, Y) ofColor(X.r * Y.r * 1.0, X.g * Y.g * 1.0, X.b * Y.b * 1.0, X.a * Y.a * 1.0)
		if(fill) {
			fillpath.setFillColor(COLMULT(styleColor, fillColor));
			fillpath.draw();
		}
		if(stroke) {
			ofColor c = COLMULT(styleColor, strokeColor);
			minpath.setStrokeColor(c);
			maxpath.setStrokeColor(c);
			minpath.setStrokeWidth(strokeWidth);
			maxpath.setStrokeWidth(strokeWidth);
			minpath.draw();
			maxpath.draw();
		}
	}
}
