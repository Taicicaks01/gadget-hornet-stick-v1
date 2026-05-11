#pragma once
#include <Arduino.h>

typedef void (*TaskCallback)(void* arg);

class TaskSchedulerClass {
public:
    bool begin();
    int scheduleOnce(uint32_t delayMs, TaskCallback cb, void* arg=nullptr);
    int schedulePeriodic(uint32_t intervalMs, TaskCallback cb, void* arg=nullptr);
    bool cancel(int id);
    void update();
private:
    struct Task {
        bool active;
        uint32_t nextRun;
        uint32_t interval;
        TaskCallback cb;
        void* arg;
        bool repeat;
    };
    static const int MAX_TASKS = 16;
    Task tasks[MAX_TASKS];
};

extern TaskSchedulerClass taskScheduler;
