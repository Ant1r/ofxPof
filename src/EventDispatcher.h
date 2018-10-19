/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "ofMain.h"

class EventData{
public:
    typedef enum {DOWN = 1, UP, MOVE, CANCEL} eventType;
    eventType type;
    int x;
    int y;
    int id;
    EventData(eventType _type, int _x = 0, int _y = 0, int _id = 0):
    type(_type), x(_x), y(_y), id(_id) {}
};

class EventDispatcher
{
	public:
		// pushing events can be done from either side
		void pushEvent(EventData::eventType type, int x = 0, int y = 0, int id = 0);
		// poping events must be done from audio (pd) side
		void popEvents();
	private:
		deque<EventData> queue;
		ofMutex mutex;
};

