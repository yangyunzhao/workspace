#pragma once
#include "config.h"
#include "task.h"

namespace core
{
    class Process
    {
    public:
        Process(UserCfg &oCfg);
        ~Process();
        void prepare();
        void run_until(int end);
        void run_timestep();
        void run_region(int regionId);
        void free_ts(TimeStep* ts);
    private:
        UserCfg &cfg;
        TQSummary TimeQ;
        TimeStep *tsPool;
    };
}