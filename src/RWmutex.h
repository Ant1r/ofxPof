/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

// readers/writer mutex
// Multiple readers are allowed at the same time, OR one single writer.

class RWmutex {
  public:	
	ofMutex R;
	ofMutex W;
	ofMutex W2;
	int Rcount;
	
	RWmutex():Rcount(0){}
	void lockR() {
		R.lock();
		Rcount++;
		if(Rcount == 1) W.lock();
		R.unlock();
	}
	void unlockR() {
		R.lock();
		Rcount--;
		if(Rcount == 0) W.unlock();
		R.unlock();
	}
	void lockW() {
		W2.lock();
		W.lock();
	}
	void unlockW() {
		W.unlock();
		W2.unlock();
	}
};

