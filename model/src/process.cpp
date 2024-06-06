#include "process.h"
#include <stdlib.h>
#include <stdio.h>

namespace core
{
    struct TaskAR *gFunc(TaskAR *ar, int arg)
    {
        printf("arg:%d\n", arg);
        return nullptr;
    }
    Process::Process(UserCfg &oCfg) : cfg(oCfg) {}
    void Process::prepare()
    {
        int ARcnt = cfg.ARcnt;
        TimeQ.head = (struct TimeStep *)malloc(sizeof(struct TimeStep));
        TimeQ.tail = TimeQ.head;

        TimeQ.head->curTime = 0;
        TimeQ.head->pre = nullptr;
        TimeQ.head->next = nullptr;
        for (int rid = 0; rid < REGION_SIZE; ++rid)
        {
            ThreadQ &tq = TimeQ.head->region[rid];
            for (int c = 0; c < ARcnt; ++c)
            {
                struct TaskAR *ar = (struct TaskAR *)malloc(sizeof(struct TaskAR));
                ar->arg = c;
                ar->func = &gFunc;
                ar->next = nullptr;
                if (tq.head)
                {
                    tq.tail->next = ar;
                    tq.tail = ar;
                }
                else
                {
                    tq.head = ar;
                    tq.tail = ar;
                }
            }
        }
    }
    void Process::run_region(int regionId)
    {
        struct ThreadQ *tq = &TimeQ.head->region[regionId];
        struct TaskAR *ar = tq->head;
        while (ar)
        {
            struct TaskAR *backup = ar;
            ar = ar->func(ar, ar->arg);
            if (ar) // new AR create,todo
            {
            }
            else
            {
                ar = backup->next;
            }
        };
    }
    void Process::run_timestep()
    {
        for (int rid = 0; rid < REGION_SIZE; ++rid)
        {
            run_region(rid);
        }
    }
    void Process::run_until(int end)
    {
        while (TimeQ.head && TimeQ.head->curTime < end)
        {
            run_timestep();
            TimeQ.head = TimeQ.head->next;
        }
    }
}