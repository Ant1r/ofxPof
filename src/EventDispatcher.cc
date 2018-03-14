/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBase.h"
#include "EventDispatcher.h"
#include "pofWin.h"
#include "RWmutex.h"

void EventDispatcher::pushEvent(eventType type, int x, int y, int id)
{
	mutex.lock();
	queue.push_back(EventData(type, x, y, id));
	mutex.unlock();
}

void EventDispatcher::popEvents()
{
	if(!mutex.try_lock()) return;

	pofBase::treeMutex.lockR();
	while(!queue.empty()) {
		EventData data = queue.front();
		queue.pop_front();
		if(pofWin::win) switch(data.type) {
			case DOWN: pofWin::win->tree_touchDown(data.x, data.y, data.id); break;
			case UP: pofWin::win->tree_touchUp(data.x, data.y, data.id); break;
			case MOVE: pofWin::win->tree_touchMoved(data.x, data.y, data.id); break;
			case CANCEL: pofWin::win->tree_touchCancel(); break;
			default: break;
		}
	}		
	pofBase::treeMutex.unlockR();
	mutex.unlock();
}

