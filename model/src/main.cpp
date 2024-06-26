
#include <iostream>
#include "config.h"
#include "process.h"
#include "CLI11.hpp"
using namespace std;
using namespace core;

int main(int argc, char **argv)
{
    CLI::App app("test simulator performance");
    argv = app.ensure_utf8(argv);
    UserCfg cfg;

    app.add_option("-n,--node", cfg.ARcnt, "specify AR node count");
    CLI11_PARSE(app, argc, argv);
    
    Process simu(cfg);
    simu.prepare();
    simu.run_until(5);
    return 0;
}