#include "schedule.h"

std::vector<int> taskQueue;
std::atomic<uint64_t> taskIndex;
std::mutex taskMut;
int64_t taskidx = 0;

void worker_fetch(int threadId, int initialCount, int subsequentCount, int protectType) {
    for (int i = 0; i < initialCount; ++ i) {
        EXEC_TASK(taskQueue[threadId * initialCount + i]);
    }

    int count = 0;
    if (protectType == 1) {
        while (true) {
            do {

            } while (false);
            uint64_t currentIndex = taskidx;
            uint32_t start = currentIndex & 0xFFFFFFFF;
            uint32_t tasksToFetch = (count == 0) ? initialCount : subsequentCount;
            uint64_t newIndex = ((currentIndex >> 32) + tasksToFetch) << 32 | (start + tasksToFetch);

            if (start >= taskQueue.size()) {
                break;
            }

            // 尝试原子地更新任务索引
            if (taskIndex.compare_exchange_strong(currentIndex, newIndex)) {
                // 计算实际需要执行的任务数，考虑边界条件
                int tasksExecuted = (int)std::min((size_t)tasksToFetch, taskQueue.size() - start);
                for (int i = 0; i < tasksExecuted; ++i) {
                    // 模拟任务执行时间
                    std::this_thread::sleep_for(std::chrono::milliseconds(taskQueue[start + i]));
                }
            }
            count++;
        }
    }
    else if (protectType == 2) {
        while (true) {
            uint64_t currentIndex = taskIndex.load();
            uint32_t start = currentIndex & 0xFFFFFFFF;
            uint32_t tasksToFetch = (count == 0) ? initialCount : subsequentCount;
            uint64_t newIndex = ((currentIndex >> 32) + tasksToFetch) << 32 | (start + tasksToFetch);

            if (start >= taskQueue.size()) {
                break;
            }

            // 尝试原子地更新任务索引
            if (taskIndex.compare_exchange_strong(currentIndex, newIndex)) {
                // 计算实际需要执行的任务数，考虑边界条件
                int tasksExecuted = (int)std::min((size_t)tasksToFetch, taskQueue.size() - start);
                for (int i = 0; i < tasksExecuted; ++i) {
                    // 模拟任务执行时间
                    std::this_thread::sleep_for(std::chrono::milliseconds(taskQueue[start + i]));
                }
            }
            count++;
        }
    }
}
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
    const auto& options = parseArguments(argc, argv);
    
    // 初始化任务队列
    taskQueue.resize(options.taskCount);
    generateRandom(options, taskQueue);

    // 线程集合
    std::vector<std::thread> threads;
    auto startTime = std::chrono::high_resolution_clock::now();

    // 启动线程
    if (options.algorithm == 1) {
        for (int i = 0; i < options.threadCount; ++i) {
            threads.push_back(std::thread(worker_fetch, i, options.initialCount, options.subsequentCount, options.protectType));
        }
    } else if (options.algorithm == 2) {
        for (int i = 0; i < options.threadCount; ++i) {
            //threads.push_back(std::thread(worker_steal, i, options.threadCount, options.numTasksToSteal, options.taskProtection));
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
