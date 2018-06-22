#include <condition_variable>
#include <queue>

using std::mutex;

template<class T>
class SyncQueue {
public:
  std::shared_ptr<T> front();
  std::shared_ptr<T> back();
  bool empty() const;
  size_t size() const;
  void push(T element);
  std::shared_ptr<T> pop();
  std::queue<T> operator=( const std::queue<T>& other );

private:
    std::condition_variable signal;
	mutex block;
	std::queue<T> container;
};

template <typename T>
std::queue<T> SyncQueue<T>::operator=( const std::queue<T>& other ) {
    std::lock_guard<mutex> lg(block);
    container=other;
}

template <typename T>
std::shared_ptr<T> SyncQueue<T>::front() {
    std::unique_lock<mutex> ul(block);
		if (container.empty()) {
			return nullptr;
		}
    std::shared_ptr<T> result = std::make_shared<T>(container.front());
		return result;
}

template <typename T>
std::shared_ptr<T> SyncQueue<T>::back() {
     std::unique_lock<mutex> ul(block);
		if (container.empty()) {
			return nullptr;
		}
    std::shared_ptr<T> result = std::make_shared<T>(container.back());
		return result;
}

template <typename T>
bool SyncQueue<T>::empty() const {
    return container.empty();
}

template <typename T>
size_t SyncQueue<T>::size() const {
    return container.size();
}

template <typename T>
void SyncQueue<T>::push(T element) {
    std::lock_guard<mutex> lg(block);
    container.push(element);
}

template <typename T>
std::shared_ptr<T> SyncQueue<T>::pop() {
    std::unique_lock<std::mutex> ul(block);
		if (container.empty()) {
			return nullptr;
		}
		std::shared_ptr<T> result = std::make_shared<T>(container.front());;
		container.pop();
		return result;
}
