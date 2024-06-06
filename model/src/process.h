#pragma once
#include "config.h"
#include "task.h"

namespace core
{
    class Process
    {
    public:
        Process(UserCfg &oCfg);
        void prepare();
        void run_until(int end);
        void run_timestep();
        void run_region(int regionId);
        UserCfg &cfg;
        TQSummary TimeQ;
    };
}