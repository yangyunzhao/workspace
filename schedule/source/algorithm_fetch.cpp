#include "schedule.h"

void worker_fetch(int threadId, Options* options) {
    for (int i = 0; i < options->initialCount; ++ i) {
        exec_task(taskQueue[threadId * options->initialCount + i]);
    }
    int count = 0;
    uint64_t currentIndex = 0;
    uint64_t newIndex = 0;

    if (options->protectType == ATOMIC) {
        while (true) {
            currentIndex = taskIndex.load();
            if (currentIndex >= taskQueue.size()) {
                break;
            }
            newIndex = currentIndex + options->subsequentCount;

            if (taskIndex.compare_exchange_strong(currentIndex, newIndex)) {
                int tasksExecuted = (int)std::min((size_t)(newIndex - currentIndex), taskQueue.size() - currentIndex);
                for (int i = 0; i < tasksExecuted; ++i) {
                    exec_task(taskQueue[currentIndex + i]);
                }
            }
            count++;
        }
    }
    else if (options->protectType == MUTEX) {
        while (true) {
            taskMut.lock();
            currentIndex = taskidx;
            if (currentIndex >= taskQueue.size()) {
                taskMut.unlock();
                break;
            }
            newIndex = currentIndex + options->subsequentCount;
            taskidx = newIndex;
            taskMut.unlock();

            int tasksExecuted = (int)std::min((size_t)(newIndex - currentIndex), taskQueue.size() - currentIndex);
            for (int i = 0; i < tasksExecuted; ++i) {
                exec_task(taskQueue[currentIndex + i]);
            }
        }
    }
}