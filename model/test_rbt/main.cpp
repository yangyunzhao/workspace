#include <iostream>
#include <random>
#include <map>
#include <chrono>
#include <cstdio>

using namespace std;
#ifdef USE_CLOCK
double cost = 0.0;
#else
uint64_t cost = 0ULL;
#endif

template <typename T = chrono::microseconds>
struct util_time
{
    util_time(const char *prefix) : pre_(prefix), start_(chrono::high_resolution_clock::now()) {}
    ~util_time()
    {
        // cout << "<" << pre_ << "> " << chrono::duration_cast<T_>(chrono::high_resolution_clock::now() - start_).count() << "\n";
        cost += chrono::duration_cast<T_>(chrono::high_resolution_clock::now() - start_).count();
    }
    using T_ = T;
    const char *pre_;
    chrono::time_point<chrono::high_resolution_clock> start_;
};

inline double gettime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000 * 1000 + now.tv_nsec * 1.0e-3;
}

struct Node
{
    int value;
    Node *next;
    Node(int v) : value(v), next(nullptr) {}
    static Node *create()
    {
        static std::random_device rd;
        static unsigned int seed = rd();
        static std::minstd_rand0 gen(seed);
        return new Node(gen());
    }
};

struct List
{
    Node *head;
    Node *tail;
};

struct transform
{
    transform(int nodeCnt, int threadCnt) : nodeCnt_(nodeCnt), threadCnt_(threadCnt)
    {
        for (int i = 0; i < nodeCnt; ++i)
        {
            tree[i] = Node::create();
        }
        threadARHead.resize(threadCnt);
    }
    void work()
    {
        vec.reserve(tree.size());
        for (auto it = tree.rbegin(); it != tree.rend(); ++it)
        {
            vec.push_back(it->second);
        }
    }
    void reset()
    {
        for (auto &e : threadARHead)
        {
            e.head = nullptr;
            e.tail = nullptr;
        }
    }
    int nodeCnt_;
    int threadCnt_;
    map<int, Node *> tree;
    std::vector<List> threadARHead;
    std::vector<Node *> vec;
};

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "usage:" << argv[0] << " 10000 200\n";
        exit(1);
    }
    int repeat = 100;
    if (argc >= 4)
    {
        repeat = stoi(argv[2]);
    }
    transform tran(stoi(argv[1]), 2);
    for (int i = 0; i < repeat; ++i)
    {
#if defined USE_CLOCK
        double t1 = gettime();
        tran.work();
        double t2 = gettime();
        cost += t2 - t1;
#else
        {
            util_time calc("transfer");
            tran.work();
            // tran.reset();
        }
#endif
    }
    printf("repeat:%d node-count:%d avg:%f\n", repeat, stoi(argv[1]), cost / (repeat + 0.0));
    return 0;
}