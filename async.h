#pragma once

#include <functional>
#include <memory>

#include "promise.h"
#include "future.h"
#include "tasks_distributor.h"
#include "threadpool.h"

enum TPolicy {
	TAsync,
	TDeferred
};


template <class TReturnType, class... TArgTypes>
std::shared_ptr<CFuture<TReturnType> > Async(
  const TPolicy type,
  std::shared_ptr<CThreadPool> threadPool,
  std::function<TReturnType(TArgTypes...)> function,
  TArgTypes... args) {
    std::shared_ptr<CTasksDistributor<TReturnType> > task(
        new CTasksDistributor<TReturnType>(threadPool, std::bind(function, args...))
    );

    if ((!type && !threadPool->HasFreeThreads()) || type == TDeferred) {
        return task->Start();
    } else {
        return task->Distribute();
    }

}
