#include "TaskScheduler.h"
#include <Arduino.h>

TaskSchedulerClass taskScheduler;

bool TaskSchedulerClass::begin() {
    for (int i = 0; i < MAX_TASKS; ++i) tasks[i].active = false;
    return true;
}

int TaskSchedulerClass::scheduleOnce(uint32_t delayMs, TaskCallback cb, void* arg) {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (!tasks[i].active) {
            tasks[i].active = true;
            tasks[i].nextRun = millis() + delayMs;
            tasks[i].interval = 0;
            tasks[i].cb = cb;
            tasks[i].arg = arg;
            tasks[i].repeat = false;
            return i;
        }
    }
    return -1;
}

int TaskSchedulerClass::schedulePeriodic(uint32_t intervalMs, TaskCallback cb, void* arg) {
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (!tasks[i].active) {
            tasks[i].active = true;
            tasks[i].nextRun = millis() + intervalMs;
            tasks[i].interval = intervalMs;
            tasks[i].cb = cb;
            tasks[i].arg = arg;
            tasks[i].repeat = true;
            return i;
        }
    }
    return -1;
}

bool TaskSchedulerClass::cancel(int id) {
    if (id < 0 || id >= MAX_TASKS) return false;
    tasks[id].active = false;
    return true;
}

void TaskSchedulerClass::update() {
    uint32_t now = millis();
    for (int i = 0; i < MAX_TASKS; ++i) {
        if (!tasks[i].active) continue;
        // handle wrap-around
        if ((int32_t)(now - tasks[i].nextRun) >= 0) {
            if (tasks[i].cb) tasks[i].cb(tasks[i].arg);
            if (tasks[i].repeat) {
                tasks[i].nextRun = now + tasks[i].interval;
            } else {
                tasks[i].active = false;
            }
        }
    }
}
