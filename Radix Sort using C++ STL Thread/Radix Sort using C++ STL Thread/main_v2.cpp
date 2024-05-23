#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <optional>

class ThreadManager {
private:
	std::vector<std::thread> threads;
public:
	int threadNum = std::thread::hardware_concurrency();

	void AssignThread(std::function<void()> task) {
		for (int i = 0; i < threadNum; ++i)
			threads.emplace_back(task);
	}
};

class TaskManager {
private:
	std::queue<std::function<void()>> taskQueue;
	std::mutex mu;
	ThreadManager threadMgr;
	int* taskRangeArr;
	int* taskRangeIndex;
public:
	void Push(std::function<void()> task) {
		std::unique_lock<std::mutex> lock(mu);
		taskQueue.push(task);
		lock.unlock();
	}

	std::function<void()> Pop() {
		std::unique_lock<std::mutex> lock(mu);
		if (taskQueue.empty())
			return {};
		auto task = taskQueue.front();
		taskQueue.pop();
		lock.unlock();
		return task;
	}

	// 루프할 때 여러 스레드가 동시에 처리하도록 루프 구역을 정해주는 함수
	int* LoopPartition(int loopCount) {
		int threadNum = threadMgr.threadNum;
		taskRangeArr = new int(threadNum + 1);
		int taskRange = loopCount / threadNum;

		for (int i = 0; i < threadNum; i++) {
			taskRangeArr[i] = taskRange;
		}
		taskRangeArr[threadNum] = loopCount - (loopCount / threadNum);
		return taskRangeArr;
	}

	int GetThreadNum() {
		return threadMgr.threadNum;
	}


	// 태스크를 루프 구역에 따라 스레드를 할당하여 작업을 실행시키는 함수
	void Work(std::function<void()> task, int* taskRangeArr) {
		auto task = Pop();
		threadMgr.AssignThread(task);
	}
};

class RadixVariable {
private:
	int max;
public:

};

void SearchMax(int* arr, int* taskRangeArr, int threadNum) {
	int max;
	for (int i = 0; i < threadNum; i++) {
		for (int j = taskRangeArr[i]; j < taskRangeArr[i+1]; j++) {
			if (max < arr[j])
				max = arr[j];
		}
	}
		
}

void RadixSort(int* arr, int size) {
	TaskManager taskMgr;
	RadixVariable radixVar;
	taskMgr.Push([=, &taskMgr] { SearchMax(arr, taskMgr.LoopPartition(size), taskMgr.GetThreadNum()); });
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
	RadixSort(arr, size);

	std::cout << arr[size / 2];
}