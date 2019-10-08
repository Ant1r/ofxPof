/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"

class pofScope;

class pofScope: public pofBase {
	public:
		pofScope(t_class *Class, float w=0, float h=0, int len=0):
			pofBase(Class),width(w), height(h),
			bufLen(len), bufIndex(0), compute(false), readPeaks(false),
			curve(0), stroke(1), fill(1), strokeColor(1,1,1,1), fillColor(1,1,1,1), strokeWidth(1)
		{
			curWidth = int(width);
			minBuf = new float[curWidth];
			maxBuf = new float[curWidth];
			std::fill_n(minBuf, curWidth, 0);
			std::fill_n(maxBuf, curWidth, 0);
		}

		~pofScope() { 
			delete [] minBuf;
			delete [] maxBuf;
		}
		
		virtual void draw();
		static void setup(void);
		
		float width, height;
		int bufLen; // nb samples to draw
		int bufCount;
		int bufIndex;
		float min, max;
		float *minBuf, *maxBuf;
		int curWidth;
		bool compute;
		bool once;
		ofMutex Mutex;
		bool readPeaks;
		t_word *peaksVec;
		float peaksFrom;
		float peaksLen;
		bool updateGUI;
		bool curve;
		bool stroke;
		bool fill;
		ofFloatColor strokeColor;
		ofFloatColor fillColor;
		float strokeWidth;
		ofPath minpath, maxpath, fillpath;
};



