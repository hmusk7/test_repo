#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>

class ThreadManager {
private:
	std::vector<std::thread> threads;
	std::mutex mu;
	std::condition_variable cv;
	std::queue<std::function<void()>> taskQueue;
	int threadNum = std::thread::hardware_concurrency();

public:
	bool isTaskDone = false;

	void Initialize() {
		for (int i = 0; i < threadNum; ++i) {
			threads.emplace_back([this, i]() { this->Work(i); });
		}
	}

	void Work(int index) {
		while (!isTaskDone ) {
			std::unique_lock<std::mutex> lock(mu);
			printf("thread %d waiting\n", index);
			cv.wait(lock, [this, index]() { std::cout << "thread " << index << " launched" << std::endl; return !taskQueue.empty(); });
			TaskPop(std::move(lock), index);
		}
	}

	void TaskPush(std::function<void()> task) {
		std::unique_lock<std::mutex> lock(mu);
		taskQueue.push(task);
		printf("Task Pushed\n");
		cv.notify_all();
	}

	void TaskPop(std::unique_lock<std::mutex>&& lock, int index) {
		//std::unique_lock<std::mutex> lock(mu);
		auto task = taskQueue.front();
		taskQueue.pop();
		lock.unlock();
		task();
		printf("task is done: thread %d\n", index);
	}
};

class RadixVariable {
public:
	int max;
	int exp;
};

void SearchMax(int& max, int* arr, int size) {
	int m = arr[0];
	for (int i = 1; i < size; i++) {
		if (m < arr[i])
			m = arr[i];
	}

	max = m;
}

void SearchExp(int& max, int& exp) {
	int count = 0;
	for (int e = 1; (max / e) > 0; e *= 10)
		count++;

	exp = count;
}

void RadixSort(ThreadManager& threadMgr, RadixVariable& radixVar, int* arr, int size) {
	threadMgr.TaskPush([=, &radixVar] { SearchMax(radixVar.max, arr, size); });
	threadMgr.TaskPush([&radixVar] { SearchExp(radixVar.max, radixVar.exp); });
	threadMgr.TaskPush([&radixVar] { std::cout << "radixVar.exp: " << radixVar.exp << std::endl; });
}

int main()
{
	srand(0);

	int size;
	std::cin >> size;
	int* arr = new int[size];
	for (int i = 0; i < size; i++)
		arr[i] = rand();

	// Sort arr !!
	ThreadManager threadMgr;
	RadixVariable radixVar;
	threadMgr.Initialize();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	RadixSort(threadMgr, radixVar, arr, size);
	while (!threadMgr.isTaskDone) {

	}

	std::cout << arr[size / 2];
}