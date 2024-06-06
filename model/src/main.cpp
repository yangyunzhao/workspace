
#include <iostream>
#include "config.h"
#include "process.h"
using namespace std;
using namespace core;

int main()
{
    UserCfg cfg;
    cfg.ARcnt = 10;
    Process simu(cfg);
    simu.prepare();
    simu.run_until(5);
    return 0;
}