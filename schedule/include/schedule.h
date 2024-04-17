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

struct Options {
    int algorithm;
    int randomType;
    int min;  // Min for Uniform
    int max;  // Max for Uniform
    int mean; // Mean for Normal
    int stddev; // Stddev for Normal
    int taskCount;
    int threadCount;
    int initialCount;
    int subsequentCount;
    int numTasksToSteal;
    int taskProtection;
    int protectType;
};

void generateRandom(const Options& options, std::vector<int>& taskQueue);

Options parseArguments(int argc, char* argv[]);

#define EXEC_TASK(task) std::this_thread::sleep_for(std::chrono::milliseconds(task))