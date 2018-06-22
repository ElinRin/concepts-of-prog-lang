#pragma once

#include <atomic>
#include <functional>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>

#include "promise.h"

template <typename T>
struct PromiseResult;

template<class T>
class CPromise;

template <typename T>
class CFuture {
public:
    CFuture(const std::shared_ptr<PromiseResult<T> >& _result,
            const std::shared_ptr<std::mutex>& _mutex)
        : result(_result),
          mutex(_mutex) {}

    std::shared_ptr<T> Get() const;
    std::shared_ptr<T> TryGet() const;
    template <class TReturnType, class... TArgTypes>
    std::shared_ptr<CFuture<TReturnType> > then( std::function<TReturnType(TArgTypes...)> function) const;


    bool IsFinished() const;

private:
    std::shared_ptr<PromiseResult<T> > result;
    std::shared_ptr<std::mutex> mutex;
};

template <typename T>
template <class TReturnType, class... TArgTypes>
std::shared_ptr<CFuture<TReturnType> > CFuture<T>::
  then( std::function<TReturnType(TArgTypes...)> function ) const {

  	auto currentFuture = *this;
    CPromise<T> promise{};
    auto future = promise.GetFuture();
    try {
    promise.SetValue( function( currentFuture.Get() ) );
    } catch( std::exception& exception ) {
    promise.SetException( exception );
    }
    return future;
}

template <typename T>
bool CFuture<T>::IsFinished() const {
    bool isFinished = mutex->try_lock();
    if (isFinished) {
        mutex->unlock();
    }
    return isFinished;
}

template <typename T>
std::shared_ptr<T> CFuture<T>::Get() const {
  mutex->lock();
  mutex->unlock();
  if (result->exception) {
      throw *(result->exception);
  }
  return result->value;
}

template <typename T>
std::shared_ptr<T> CFuture<T>::TryGet() const {
    if (mutex->try_lock()) {
      mutex->unlock();
      if (result->exception) {
          throw *(result->exception);
      }
      return result->value;
    }
    return nullptr;
}
