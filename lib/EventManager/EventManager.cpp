#include "EventManager.h"
#include <Arduino.h>

EventManagerClass eventManager;

bool EventManagerClass::begin() {
    qHead = qTail = 0;
    subCount = 0;
    return true;
}

bool EventManagerClass::publish(const Event& ev) {
    int next = (qTail + 1) % QUEUE_SIZE;
    if (next == qHead) return false; // full
    queue[qTail] = ev;
    queue[qTail].timestamp = millis();
    queue[qTail].data = ev.data;
    qTail = next;
    return true;
}

bool EventManagerClass::subscribe(EventCallback cb, EventType filter) {
    if (subCount >= MAX_SUB) return false;
    subs[subCount] = cb;
    subFilter[subCount] = filter;
    subCount++;
    return true;
}

void EventManagerClass::process() {
    while (qHead != qTail) {
        Event ev = queue[qHead];
        qHead = (qHead + 1) % QUEUE_SIZE;
        for (int i = 0; i < subCount; ++i) {
            if (subFilter[i] == 0xFFFF || subFilter[i] == ev.type) {
                if (subs[i]) subs[i](ev);
            }
        }
    }
}
