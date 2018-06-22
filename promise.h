#pragma once

#include <exception>
#include <memory>
#include <mutex>
#include <typeinfo>

#include "future.h"


template <typename T>
struct PromiseResult {
    std::shared_ptr<T> value;
    std::shared_ptr<std::logic_error> exception;
};

template <typename T>
class CPromise {
public:
    CPromise()
        : result(new PromiseResult<T>()),
          mutex(new std::mutex()) {
        mutex->lock();
        future = std::make_shared<CFuture<T> >(result, mutex);
    }

    void SetValue(const T& _value);
    void SetException(const std::exception& _exception);
    std::shared_ptr<CFuture<T> > GetFuture() const;

private:
    std::shared_ptr<PromiseResult<T> > result;
    std::shared_ptr<std::mutex> mutex;
    std::shared_ptr<CFuture<T> > future;
};

template <typename T>
void CPromise<T>::SetValue(const T& _value) {
    result->value = std::make_shared<T>(_value);
    mutex->unlock();
}

template <typename T>
void CPromise<T>::SetException(const std::exception& _exception) {
    result->exception = std::make_shared<std::logic_error>(_exception.what());
    mutex->unlock();
}

template <typename T>
std::shared_ptr<CFuture<T> > CPromise<T>::GetFuture() const {
    return std::shared_ptr<CFuture<T> >(future);
}
