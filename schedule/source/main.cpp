#include "schedule.h"

#if 0
struct TaskQueue {
    std::vector<int> tasks;
    std::atomic<size_t> index;  // 当前任务索引
};
std::vector<TaskQueue> queues;

bool stealTasks(int fromThreadId, std::vector<int>& localQueue, const Options& options) {
    TaskQueue& srcQueue = queues[fromThreadId];
    size_t oldIndex = srcQueue.index.load();
    if (oldIndex < options.taskProtection || oldIndex <= srcQueue.tasks.size() - stealTaskCount) {
        return false;
    }

    size_t newIndex = oldIndex - stealTaskCount;
    if (srcQueue.index.compare_exchange_strong(oldIndex, newIndex)) {
        for (size_t i = 0; i < stealTaskCount; ++i) {
            localQueue.push_back(srcQueue.tasks[newIndex + i]);
        }
        return true;
    }

    return false;
}

void worker_steal(int threadId, int threadCount, int numTasksToSteal, int taskProtection) {
    std::vector<int>& localQueue = queues[threadId].tasks;
    size_t& localIndex = queues[threadId].index;

    while (true) {
        if (localIndex < localQueue.size()) {
            size_t taskIndex = localIndex++;
            if (taskIndex < localQueue.size()) {
                sleep(localQueue[taskIndex]);  // 执行任务
            }
        } else {
            // 尝试从其他线程偷取任务
            bool stolen = false;
            for (int i = 0; i < threadCount; ++i) {
                if (i != threadId) {
                    stolen = stealTasks(i, localQueue);
                    if (stolen) break;
                }
            }
            if (!stolen) break;  // 如果没有任务可以偷，结束线程
        }
    }
}
#endif 
int main(int argc, char* argv[]) {
    auto options = parseArguments(argc, argv);
    
    // 初始化任务队列
    generateRandom(options, taskQueue, taskList);

    // 线程集合
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();

    // 启动线程
    if (options->algorithm == FETCH) {
        taskIndex = options->threadCount * options->initialCount;
        taskidx = options->threadCount * options->initialCount;
        for (int i = 0; i < options->threadCount; ++i) {
            threads.push_back(std::thread(worker_fetch, i, options));
        }
    } else if (options->algorithm == STEAL) {
        for (int i = 0; i < options->threadCount; ++i) {
            threads.push_back(std::thread(worker_steal, i, options));
        }
    }

    // 等待所有线程完成
    for (auto &th : threads) {
        th.join();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    std::cout << "Total time taken: " << duration << " milliseconds.\n";
    return 0;
}
