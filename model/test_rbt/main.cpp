#include <iostream>
#include <random>
#include <map>
#include <chrono>
#include <cstdio>

using namespace std;
uint64_t cost = 0ULL;

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
    if (argc < 3)
    {
        cout << "usage:" << argv[0] << " 10000 4\n";
        exit(1);
    }
    int repeat = 100;
    if (argc >= 4)
    {
        repeat = stoi(argv[3]);
    }
    transform tran(stoi(argv[1]), stoi(argv[2]));
    for (int i = 0; i < repeat; ++i)
    {
        {
            util_time calc("transfer");
            tran.work();
            //tran.reset();
        }
        
    }
    printf("repeat:%d node-count:%d thread-cnt:%d avg:%f\n", repeat, stoi(argv[1]), stoi(argv[2]), cost / (repeat + 0.0));
    return 0;
}