#include "schedule.h"

void generateRandom(const Options& options, std::vector<int>& taskQueue) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (options.randomType == 1) {
        std::uniform_int_distribution<int> uniform(options.min, options.max);
        for (int i = 0; i < options.taskCount; ++i) {
            taskQueue.push_back(uniform(gen));
        }
    } else if (options.randomType == 2) {
        std::normal_distribution<double> normal(options.mean, options.stddev);
        for (int i = 0; i < options.taskCount; ++i) {
            taskQueue.push_back(std::max(0, (int)normal(gen)));
        }
    }
}

Options parseArguments(int argc, char* argv[]) {
    try {
        cxxopts::Options options("TaskScheduler", "A sample task scheduler program");

        options.add_options()
            ("a,algorithm", "Algorithm type (1: Sequential fetch, 2: Work stealing)", cxxopts::value<int>())
            ("r,randomType", "Random type (1: Uniform, 2: Normal)", cxxopts::value<int>())
            ("min", "Min value for Uniform distribution", cxxopts::value<int>()->default_value("0"))
            ("max", "Max value for Uniform distribution", cxxopts::value<int>()->default_value("0"))
            ("mean", "Mean value for Normal distribution", cxxopts::value<int>()->default_value("0"))
            ("stddev", "Standard deviation for Normal distribution", cxxopts::value<int>()->default_value("0"))
            ("t,taskCount", "Total number of tasks", cxxopts::value<int>())
            ("T,threadCount", "Number of threads", cxxopts::value<int>())
            ("i,initialCount", "Initial task count for algorithm 1", cxxopts::value<int>())
            ("s,subsequentCount", "Subsequent task count for algorithm 1", cxxopts::value<int>())
            ("n,numTasksToSteal", "Number of tasks to steal for algorithm 2", cxxopts::value<int>())
            ("m,taskProtection", "Task protection count for algorithm 2", cxxopts::value<int>())
            ("p,protectType", "Protect type (1: mutex, 2: atomic)", cxxopts::value<int>())
            ("h,help", "Print usage");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        Options opts;
        opts.algorithm = result["algorithm"].as<int>();
        opts.randomType = result["randomType"].as<int>();
        if (opts.algorithm != 1 && opts.algorithm != 2) {
            throw std::runtime_error("Invalid algorithm type. Must be 1 or 2.");
        }
        if (opts.randomType != 1 && opts.randomType != 2) {
            throw std::runtime_error("Invalid random type. Must be 1 or 2.");
        }
        if (opts.algorithm == 1) {
            if (result.count("initialCount") == 0 || result.count("subsequentCount") == 0) {
                throw std::runtime_error("Algorithm 'Sequential fetch' requires 'initialCount' or 'subsequentCount'.");
            }
            opts.initialCount = result["initialCount"].as<int>();
            opts.subsequentCount = result["subsequentCount"].as<int>();
        } else if (opts.algorithm == 2) {
            if (result.count("numTasksToSteal") == 0 || result.count("taskProtection") == 0) {
                throw std::runtime_error("Algorithm 'Work stealing' requires 'numTasksToSteal' or 'taskProtection'.");
            }
            opts.numTasksToSteal = result["numTasksToSteal"].as<int>();
            opts.taskProtection = result["taskProtection"].as<int>();
        }
        if (opts.randomType == 1) {
            if (result.count("min") == 0 || result.count("max") == 0) {
                throw std::runtime_error("Uniform distribution requires 'min' and 'max' values.");
            }                
            opts.min = result["min"].as<int>();
            opts.max = result["max"].as<int>();
        }
        if (opts.randomType == 2 ) {
            if (result.count("mean") == 0 || result.count("stddev") == 0) {
                throw std::runtime_error("Normal distribution requires 'mean' and 'stddev' values.");
            } 
            opts.mean = result["mean"].as<int>();
            opts.stddev = result["stddev"].as<int>();
        }
        
        opts.taskCount = result["taskCount"].as<int>();
        opts.threadCount = result["threadCount"].as<int>();
        opts.protectType = result["protectType"].as<int>();

        return opts;
    }
    catch (const cxxopts::exceptions::exception& e) {
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        exit(1);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Validation error: " << e.what() << std::endl;
        exit(1);
    }
}

void exec_task(int load) {
    auto start = std::chrono::high_resolution_clock::now();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::milliseconds elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    while (elapsed_milliseconds.count() < load) {
        for (int i = 0; i < 1000; i++) {}  // Busy loop to simulate load
        end = std::chrono::high_resolution_clock::now();
        elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    }
}