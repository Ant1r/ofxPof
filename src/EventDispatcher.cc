/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofBase.h"
#include "EventDispatcher.h"
#include "pofWin.h"
#include "RWmutex.h"

void EventDispatcher::pushEvent(EventData::eventType type, int x, int y, int id)
{
	mutex.lock();
	queue.push_back(EventData(type, x, y, id));
	mutex.unlock();
	if(!pofBase::pdProcessesTouchEvents) popEvents();
}

void EventDispatcher::popEvents()
{
	while(mutex.try_lock()) {
		if(queue.empty()) {
			mutex.unlock();
			return;
		}
		EventData data = queue.front();
		queue.pop_front();
		mutex.unlock();
		
		pofBase::treeMutex.lockR();
		if(pofWin::win) switch(data.type) {
	        case EventData::DOWN: 
#ifdef DEBUG_TOUCH
	        	pofBase::touchdownCount = 0;
#endif
	        	pofWin::win->tree_touchDown(data.x, data.y, data.id);
#ifdef DEBUG_TOUCH
	        	ofLogNotice("touchdownCount") << pofBase::touchdownCount;
#endif
	        	break;
			case EventData::UP: pofWin::win->tree_touchUp(data.x, data.y, data.id); break;
			case EventData::MOVE: pofWin::win->tree_touchMoved(data.x, data.y, data.id); break;
			case EventData::CANCEL: pofWin::win->tree_touchCancel(); break;
			default: break;
		}		
		pofBase::treeMutex.unlockR();
	}	
}

