#include <stdexcept>
#include <string>
#include <iostream>

#include "promise.h"
#include "future.h"
#include "async.h"
#include "threadpool.h"

int main(int argc, char** argv) {
    std::string str;

  CPromise<int> promise1{};
	auto future1 = promise1.GetFuture();
	promise1.SetValue( 1 );
	std::cout << *future1->Get() << '\n';

  CPromise<int> promise2{};
	auto future2 = promise2.GetFuture();
	promise2.SetException(  std::logic_error("Ошибка") );

	try {
    str =  *future2->Get() ? "2, но вы не должны этого видеть" : "Что-то пошло не так";
		std::cout << str;
	} catch( const std::exception& exception ) {
		std::cout << exception.what() << '\n';
	}

  CPromise<int> promise3{};
	auto future3 = promise3.GetFuture();
	promise3.SetValue( 3 );
	std::cout << *future3->TryGet() << '\n';

  CPromise<int> promise4{};
	auto future4 = promise4.GetFuture();
  str = future4->TryGet() ? "Что-то пошло не так" : "4";
  std::cout << str << '\n';

  std::shared_ptr<CThreadPool> threadPool = std::make_shared<CThreadPool>();
  std::function<int(int)> functionId = [](int value) { return value; };
  std::shared_ptr<CFuture<int> > future5 = Async<int, int>(TDeferred, threadPool, functionId,5);
  std::shared_ptr<int> result = future5->TryGet();
  std::cout << *result << '\n';

  CPromise<int> promise6{};
	auto future6 = promise6.GetFuture();
	promise6.SetValue( 6 );
  std::cout << *future6->Get() << '\n';

  std::function<int(std::shared_ptr<int>)> function = [](std::shared_ptr<int> a) {
        return ++(*a);
    };
  auto future7 = future6->then(function);
	std::cout << *future7->Get() << '\n';

  return 0;
}
