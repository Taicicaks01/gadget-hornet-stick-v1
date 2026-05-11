#pragma once
#include <Arduino.h>

typedef uint16_t EventType;

struct Event {
    EventType type;
    uint32_t timestamp;
    void* data;
};

typedef void (*EventCallback)(const Event& ev);

class EventManagerClass {
public:
    bool begin();
    bool publish(const Event& ev);
    bool subscribe(EventCallback cb, EventType filter=0xFFFF);
    void process();
private:
    static const int QUEUE_SIZE = 24;
    Event queue[QUEUE_SIZE];
    int qHead = 0;
    int qTail = 0;

    static const int MAX_SUB = 12;
    EventCallback subs[MAX_SUB];
    EventType subFilter[MAX_SUB];
    int subCount = 0;
};

extern EventManagerClass eventManager;
