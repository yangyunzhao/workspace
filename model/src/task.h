#pragma once
#include "commondef.h"
namespace core
{
    struct TaskAR;
    using TASK_FUNC = TaskAR *(TaskAR *);
    struct TaskAR
    {
        TASK_FUNC func;
        TaskAR *next;
    };

    struct ThreadQ{
        struct TaskAR * head;
        struct TaskAR * tail;
    };

    struct TimeStep{
        struct TimeStep * next;
        struct TimeStep * pre;
        struct ThreadQ    region[REGION_SIZE];
    };

    struct TQSummary{
        struct TimeStep * head;
        struct TimeStep * tail;
    };
}