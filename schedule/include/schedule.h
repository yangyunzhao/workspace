#pragma once

#include <iostream>
#include "cxxopts.hpp"
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <chrono>
#include <string>
#include <mutex>
#include <list>

enum E_Protect_Type{
    ATOMIC = 1,
    MUTEX = 2
};

enum E_Algorithm_Type{
    FETCH = 1,
    STEAL = 2,
    MIXED = 3
};

enum E_Random_Type{
    UNIFORM = 1,
    NORMAL = 2
};

struct Options {
    int algorithm;          // 1:fetch;2:steal;3:mixed
    int randomType;         // 1:Uniform;2:Normal
    int min;                // Min for Uniform
    int max;                // Max for Uniform
    int mean;               // Mean for Normal
    int stddev;             // Stddev for Normal
    int taskCount;
    int threadCount;
    int initialCount;       // fetch: 
    int subsequentCount;    // fetch:
    int numTasksToSteal;    // steal:
    int taskProtection;     // steal:
    int protectType;        // 1:atomic;2:mutex
};

// for fetch
extern std::vector<int> taskQueue;
extern std::atomic<uint64_t> taskIndex;
extern std::mutex taskMut;
extern int64_t taskidx;

// for steal
struct TaskQueue {
    std::list<int> tasks;
    std::atomic<size_t> index;  // 当前任务索引
};
extern std::list<int> taskList;
extern std::vector<TaskQueue> queues;

void generateRandom(Options* options, std::vector<int>& taskQueue, std::list<int>& taskList);

Options* parseArguments(int argc, char* argv[]);

void worker_fetch(int threadId, Options* options);
void worker_steal(int threadId, Options* options);
void worker_mixed(int threadId, Options* options);
void exec_task(int load);
