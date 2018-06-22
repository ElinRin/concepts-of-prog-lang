#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <vector>

#include "promise.h"
#include "future.h"
#include "threadpool.h"


template <typename TResult>
class CTasksDistributor {
public:
    CTasksDistributor(
      std::shared_ptr<CThreadPool> _threadPool,
      const std::function<TResult()>& _function)
        : threadPool(_threadPool),
          function(_function),
          promise(new CPromise<TResult>()) {
        future = promise->GetFuture();
    }

    std::shared_ptr<CFuture<TResult> > Start() const;
    std::shared_ptr<CFuture<TResult> > Distribute() const;

private:
    std::shared_ptr<CThreadPool> threadPool;
    std::function<TResult()> function;
    std::shared_ptr<CPromise<TResult> > promise;
    std::shared_ptr<CFuture<TResult> > future;
};

template <typename TResult>
std::shared_ptr<CFuture<TResult> > CTasksDistributor<TResult>::Start() const {
    if (!future->IsFinished()) {
      try {
          promise->SetValue(function());
      } catch (const std::exception& error) {
          promise->SetException(error);
      }
    }
    return future;
}

template <typename TResult>
std::shared_ptr<CFuture<TResult> > CTasksDistributor<TResult>::Distribute() const {
    if (!future->IsFinished()) {
        std::shared_ptr<CPromise<TResult> > p = promise;
        std::function<TResult()> f = function;

        threadPool->AddTask([p, f]() {
            try {
                p->SetValue(f());
            } catch (const std::exception& error) {
                p->SetException(error);
            }
        });
    }
    return future;
}
