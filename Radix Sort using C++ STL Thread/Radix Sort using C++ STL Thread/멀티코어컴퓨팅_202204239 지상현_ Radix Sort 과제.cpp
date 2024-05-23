// 제가 병렬화하고자 했던 부문은 배열의 size 만큼 루프하는 과정에서,
// 루프의 크기를 파티셔닝하여 각 스레드가 루프를 수행할 수 있도록 하고 싶었습니다.
// 쉬지않고 코드를 짜고 투자했지만.. 지금 지각 제출임에도 현재 저녁 11시 50분 더이상 병렬화 코드 짤 수가 없어 중지했습니다.
// 그래서 모든 병렬화하고 싶은 부분들 중 배열의 최대값을 찾는 과정만 병렬화해보았습니다.
// 하지만 병렬화 이전 순차적인 Radix Sort 를 먼저 짰을 때는 값이 정상 출력 되었는데, 병렬화 이후에는 정상 출력이 되지 않네요..

#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <cmath>

// 배열에서 최댓값을 찾는 함수입니다. 병렬화하였습니다.
int SearchMax(int* arr, int begin, int end) {

	long long int max = arr[0];

	for (int i = begin; i < end; ++i) {
		if (max < arr[i])
			max = arr[i];
	}

	return max;
}

// 배열의 최댓값의 자릿수를 구하는 함수입니다. 여기서 나온 자릿수만큼 루프하여 Bucketing 합니다.
int SearchMaxDigit(std::vector<std::thread>& threads, int threadNum, TaskQueue& taskQueue, int* arr, int size) {
	
	// 각 스레드가 배열의 구간을 나눠 최댓값을 찾고, maxArr 에 저장합니다.
	// 이후 maxArr 의 값을 메인 스레드에서 비교해서 max 값을 찾아냅니다.
	int threadIndex = 0;

	// 파트는 배열 전체 크기에서 스레드의 개수로 나눠서 구합니다.
	// 이 때, 딱 맞게 안나눠 떨어질 수 있으므로 마지막 파트는 size % threadNum 하여 나눠 떨어지는지 확인하고,
	// 나눠 떨어지지 않으면 마지막 나머지 파트도 검사하게 합니다.
	long long int* maxArr = new long long int[threadNum + 1];
	int part = size / threadNum;
	for (int i = 0; i < threadNum; i++) {
		threads.emplace_back([&taskQueue] { Work(taskQueue); });
		taskQueue.Push([&] { maxArr[threadIndex] = SearchMax(arr, i * part, (i + 1) * part); });
		threadIndex++;
	}

	if ((size % threadNum) != 0) {
		taskQueue.Push([&] { maxArr[threadIndex] = SearchMax(arr, threadNum * part, size); });
	}

	else
		maxArr[threadIndex] = 0;

	taskQueue.Notify();

	long long int max = maxArr[0];
	for (int i = 1; i < threadNum + 1; i++) {
		if (max < maxArr[i])
			max = maxArr[i];
	}

	int maxDigit = 0;

	for (long long int radix = 1; (long long int)(max / radix) > 0; radix *= 10)
		maxDigit++;

	return maxDigit;
}

// 각 0 ~ 9 까지의 숫자를 카운트하여 0 ~ 9 번 버켓의 할당할 크기를 얻어옵니다.
void CalBucketSize(int* arr, int size, int currDigit) {

	bucketSize = new int[10];

	for (int i = 0; i < 10; ++i)
		bucketSize[i] = 0;

	// 병렬화할 것
	for (int i = 0; i < size; ++i) {
		int num = arr[i] / currDigit;
		num %= 10;
		bucketSize[num]++;
	}
}

// std::vector 컨테이너에 0 ~ 9번 버켓을 실제로 할당해줍니다.
void AllocateBuckets() {
	for (int i = 0; i < 10; ++i) {
		int* bkt = new int[bucketSize[i]];
		buckets.push_back(bkt);
	}
}

// 버켓에 각 자릿수 값을 
void StoreBuckets(int* arr, int size, int currDigit) {

	int currIndex[10] = { 0 };


	// 병렬화할 것
	for (int i = 0; i < size; ++i) {
		int num = arr[i] / currDigit;
		// 0 ~ 9 값 추출
		num %= 10;
		// 0 ~ 9 값에 해당되는 버켓에 값을 넣어줌
		buckets[num][currIndex[num]] = arr[i];
		currIndex[num]++;
	}
}

// 버켓들을 하나로 합쳐 한 루프를 종료하고, 이를 반복하여 Radix Sorting 합니다.
void ConcatenateBuckets(int* arr, int size) {

	int* mergedBucket = new int[size];
	int mergeSize = 0;

	for (int i = 0; i < 10; ++i) {
		std::copy(buckets[i], buckets[i] + bucketSize[i], mergedBucket + mergeSize);
		mergeSize += bucketSize[i];
		delete[] buckets[i];
	}

	// 병렬화할 것
	for (int i = 0; i < size; ++i) {
		arr[i] = mergedBucket[i];
	}

	delete[] bucketSize;
	delete[] mergedBucket;
	buckets.clear();
}

void SeqRadixSort(long long int* arr, int size) {

	// 최대 자릿수를 찾기 위해, 먼저 배열의 최댓값 찾기
	long long int max = arr[0];
	for (int i = 1; i < size; ++i) {
		if (max < arr[i])
			max = arr[i];
	}

	// 최댓값의 자릿수 찾기
	int maxDigit = 0;
	for (int i = 1; (max - i) > 0; i *= 10) {
		maxDigit++;
	}

	// 최댓값의 자릿수 만큼 버켓팅을 반복
	std::vector<std::vector<int>> buckets(10);	// 0 ~ 9번 버켓을 저장할 vector 컨테이너, radix 에 따라 각 버켓에 넣어줄 것임
	std::vector<int> mergedBucket; // 버켓에 분류한 후, 하나로 통합한 버켓. 정렬되어있으며, 이 버켓의 값을 배열에 넣어줄 것임
	for (int exp = 1; exp <= maxDigit; ++exp) {

		// 검사할 자리
		int currDigit = pow(10, exp);

		// arr 값을 각 버켓에 분류
		for (int i = 0; i < size; ++i) {
			int radix = arr[i] / currDigit;
			radix %= 10;
			buckets[radix].push_back(arr[i]);
		}

		// 버켓을 하나로 병합
		for (const auto& bucket : buckets) {
			mergedBucket.insert(mergedBucket.end(), bucket.begin(), bucket.end());
		}

		// 배열을 정렬된 상태로 갱신
		for (int i = 0; i < size; ++i) {
			arr[i] = mergedBucket[i];
		}

		// 버켓 전부 초기화
		for (int i = 0; i < 10; ++i) {
			buckets[i].clear();
			buckets[i].shrink_to_fit();
		}
		mergedBucket.clear();
		mergedBucket.shrink_to_fit();
	}
}


void ParRadixSort(long long int* arr, int size) {

	int threadNum = std::thread::hardware_concurrency();
	std::vector<std::thread> threads;
	std::mutex mu;
	struct range { int begin; int end; };
	std::vector<range> workRange;

	if (size < threadNum) {
		SeqRadixSort(arr, size);
		return;
	}

	// 스레드 작업 구간 설정
	int range = size / threadNum;

	for (int i = 0; i < threadNum; ++i) {
		
		if ((i == threadNum - 1) && (size % threadNum != 0)) {
			int begin = i * range;
			int end = size;
			workRange.push_back({ begin, end });
			break;
		}

		int begin = i * range;
		int end = (i + 1) * range;
		workRange.push_back({ begin, end });
	}

	// 최대 자릿수를 찾기 위해, 먼저 배열의 최댓값 찾기
	long long int* maxArr = new long long int[threadNum];
	for (int i = 0; i < threadNum; ++i) {
		threads.emplace_back([&workRange, i, &mu, &maxArr, arr]() {
			for (int j = workRange[i].begin; j < workRange[i].end; j++) {
				if (maxArr[i] < arr[j])
					std::unique_lock<std::mutex> lock(mu);
					maxArr[i] = arr[j];
					mu.unlock();
			}
		});
	}

	for (auto& t : threads) {
		if (t.joinable()) {
			t.join();
		}
	}

	threads.clear();

	long long int max = 0;
	for (int i = 0; i < threadNum; ++i) {
		if (max < maxArr[i])
			max = maxArr[i];
	}

	delete[] maxArr;

	// 최댓값의 자릿수 찾기
	int maxDigit = 0;
	for (int i = 1; (max - i) > 0; i *= 10) {
		maxDigit++;
	}

	// 각 자리마다 Bucketing 시작
	for (int i = 0; i < maxDigit; ++i) {
		long long int currDigit = pow(radix, i);
		CalBucketSize(arr, size, currDigit);
		AllocateBuckets();
		StoreBuckets(arr, size, currDigit);
		ConcatenateBuckets(arr, size);
	}

	taskQueue.TaskDone();
	for (auto& t : threads)
		t.join();
}

int main()
{
	srand(0);

	int size;
	std::cin >> size;
	long long int* arr = new long long int[size];
	for (int i = 0; i < size; ++i)
		arr[i] = rand();

	// 순차 Radix Sorting 의 결과와 병렬 Radix Sorting 의 결과를 비교하기 위해,
	// 원래의 arr 배열을 temp 배열에 저장해두고,
	// 순차 Radix Sorting 에 의해 arr 배열이 변하면 temp 배열의 값을 다시 씌워줄 것임.
	int* temp = new int[size];
	for (int i = 0; i < size; ++i)
		temp[i] = arr[i];

	// Sort arr !!

	// 순차적인 Radix Sort
	SeqRadixSort(arr, size);
	
	// arr 초기화
	int* seqArr = new int[size];
	for (int i = 0; i < size; ++i) {
		seqArr[i] = arr[i];
		arr[i] = temp[i];
	}

	// 병렬적인 Radix Sort
	ParRadixSort(arr, size);

	std::cout << arr[size / 2];


	delete[] arr;
	delete[] temp;
	delete[] seqArr;
}