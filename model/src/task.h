#pragma once
#include "commondef.h"
namespace core
{
    struct TaskAR;
    typedef struct TaskAR *(*TASK_FUNC)(struct TaskAR *, int arg);
    // using TASK_FUNC = struct TaskAR *(TaskAR *,int arg);
    struct TaskAR
    {
        TASK_FUNC func;
        TaskAR *next;
        TaskAR *parent;
        int arg;
    };

    struct ThreadQ
    {
        struct TaskAR *head;
        struct TaskAR *tail;
    };

    struct TimeStep
    {
        struct TimeStep *next;
        struct TimeStep *pre;
        int curTime;
        struct ThreadQ region[REGION_SIZE];
    };

    struct TQSummary
    {
        struct TimeStep *head;
        struct TimeStep *tail;
    };
}