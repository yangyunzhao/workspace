#include "schedule.h"

void worker_steal(int threadId, Options* options) {
    auto& localQueue = queues[threadId];
    auto itr = localQueue.tasks.begin();
    while (true) {
        if (localIndex < localQueue.size()) {
            size_t taskIndex = localIndex++;
            if (taskIndex < localQueue.size()) {
                sleep(localQueue[taskIndex]);
            }
        }
        else {
            bool stolen = false;
            for (int i = 0; i < threadCount; ++i) {
                if (i != threadId) {
                    stolen = stealTasks(i, localQueue);
                    if (stolen) break;
                }
            }
            if (!stolen) break;
        }
    }
}