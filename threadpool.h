#pragma once
#include <atomic>
#include <functional>
#include <queue>
#include <vector>

#include "sync_queue.h"

class CThreadPool {
public:
    CThreadPool(int _threadsCount = 2);
    ~CThreadPool();

    void AddTask(const std::function<void()>& function);
    bool HasFreeThreads();

private:
    void run();
    int threadsCount;
    std::atomic<bool> shouldFinish;
    std::vector<std::thread> threads;
    SyncQueue<std::function<void()> > tasks;
    std::atomic<int> workingThreasCount{ 0 };
};

CThreadPool::CThreadPool(int _threadsCount)
    : threadsCount(_threadsCount),
      shouldFinish(false) {
    threads.reserve(threadsCount);
    for (int i = 0; i < threadsCount; ++i) {
        threads.push_back(std::thread(&CThreadPool::run, this));
    }
}

CThreadPool::~CThreadPool() {
    shouldFinish = true;
    for (unsigned int i = 0; i < threadsCount; ++i) {
        threads[i].join();
    }
}

void CThreadPool::AddTask(const std::function<void()>& function) {
    tasks.push(function);
}

bool CThreadPool::HasFreeThreads() {
	return workingThreasCount < threadsCount;
}

void CThreadPool::run() {
    while (true) {
        if (shouldFinish) {
            break;
        }
        if (tasks.front()) {
          std::function<void()> task = *(tasks.pop());
      		++workingThreasCount;
      		task();
      		--workingThreasCount;
        }

    }
}
