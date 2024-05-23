// ���� ����ȭ�ϰ��� �ߴ� �ι��� �迭�� size ��ŭ �����ϴ� ��������,
// ������ ũ�⸦ ��Ƽ�Ŵ��Ͽ� �� �����尡 ������ ������ �� �ֵ��� �ϰ� �;����ϴ�.
// �����ʰ� �ڵ带 ¥�� ����������.. ���� ���� �����ӿ��� ���� ���� 11�� 50�� ���̻� ����ȭ �ڵ� © ���� ���� �����߽��ϴ�.
// �׷��� ��� ����ȭ�ϰ� ���� �κе� �� �迭�� �ִ밪�� ã�� ������ ����ȭ�غ��ҽ��ϴ�.
// ������ ����ȭ ���� �������� Radix Sort �� ���� ®�� ���� ���� ���� ��� �Ǿ��µ�, ����ȭ ���Ŀ��� ���� ����� ���� �ʳ׿�..

#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <cmath>

// �迭���� �ִ��� ã�� �Լ��Դϴ�. ����ȭ�Ͽ����ϴ�.
int SearchMax(int* arr, int begin, int end) {

	long long int max = arr[0];

	for (int i = begin; i < end; ++i) {
		if (max < arr[i])
			max = arr[i];
	}

	return max;
}

// �迭�� �ִ��� �ڸ����� ���ϴ� �Լ��Դϴ�. ���⼭ ���� �ڸ�����ŭ �����Ͽ� Bucketing �մϴ�.
int SearchMaxDigit(std::vector<std::thread>& threads, int threadNum, TaskQueue& taskQueue, int* arr, int size) {
	
	// �� �����尡 �迭�� ������ ���� �ִ��� ã��, maxArr �� �����մϴ�.
	// ���� maxArr �� ���� ���� �����忡�� ���ؼ� max ���� ã�Ƴ��ϴ�.
	int threadIndex = 0;

	// ��Ʈ�� �迭 ��ü ũ�⿡�� �������� ������ ������ ���մϴ�.
	// �� ��, �� �°� �ȳ��� ������ �� �����Ƿ� ������ ��Ʈ�� size % threadNum �Ͽ� ���� ���������� Ȯ���ϰ�,
	// ���� �������� ������ ������ ������ ��Ʈ�� �˻��ϰ� �մϴ�.
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

// �� 0 ~ 9 ������ ���ڸ� ī��Ʈ�Ͽ� 0 ~ 9 �� ������ �Ҵ��� ũ�⸦ ���ɴϴ�.
void CalBucketSize(int* arr, int size, int currDigit) {

	bucketSize = new int[10];

	for (int i = 0; i < 10; ++i)
		bucketSize[i] = 0;

	// ����ȭ�� ��
	for (int i = 0; i < size; ++i) {
		int num = arr[i] / currDigit;
		num %= 10;
		bucketSize[num]++;
	}
}

// std::vector �����̳ʿ� 0 ~ 9�� ������ ������ �Ҵ����ݴϴ�.
void AllocateBuckets() {
	for (int i = 0; i < 10; ++i) {
		int* bkt = new int[bucketSize[i]];
		buckets.push_back(bkt);
	}
}

// ���Ͽ� �� �ڸ��� ���� 
void StoreBuckets(int* arr, int size, int currDigit) {

	int currIndex[10] = { 0 };


	// ����ȭ�� ��
	for (int i = 0; i < size; ++i) {
		int num = arr[i] / currDigit;
		// 0 ~ 9 �� ����
		num %= 10;
		// 0 ~ 9 ���� �ش�Ǵ� ���Ͽ� ���� �־���
		buckets[num][currIndex[num]] = arr[i];
		currIndex[num]++;
	}
}

// ���ϵ��� �ϳ��� ���� �� ������ �����ϰ�, �̸� �ݺ��Ͽ� Radix Sorting �մϴ�.
void ConcatenateBuckets(int* arr, int size) {

	int* mergedBucket = new int[size];
	int mergeSize = 0;

	for (int i = 0; i < 10; ++i) {
		std::copy(buckets[i], buckets[i] + bucketSize[i], mergedBucket + mergeSize);
		mergeSize += bucketSize[i];
		delete[] buckets[i];
	}

	// ����ȭ�� ��
	for (int i = 0; i < size; ++i) {
		arr[i] = mergedBucket[i];
	}

	delete[] bucketSize;
	delete[] mergedBucket;
	buckets.clear();
}

void SeqRadixSort(long long int* arr, int size) {

	// �ִ� �ڸ����� ã�� ����, ���� �迭�� �ִ� ã��
	long long int max = arr[0];
	for (int i = 1; i < size; ++i) {
		if (max < arr[i])
			max = arr[i];
	}

	// �ִ��� �ڸ��� ã��
	int maxDigit = 0;
	for (int i = 1; (max - i) > 0; i *= 10) {
		maxDigit++;
	}

	// �ִ��� �ڸ��� ��ŭ �������� �ݺ�
	std::vector<std::vector<int>> buckets(10);	// 0 ~ 9�� ������ ������ vector �����̳�, radix �� ���� �� ���Ͽ� �־��� ����
	std::vector<int> mergedBucket; // ���Ͽ� �з��� ��, �ϳ��� ������ ����. ���ĵǾ�������, �� ������ ���� �迭�� �־��� ����
	for (int exp = 1; exp <= maxDigit; ++exp) {

		// �˻��� �ڸ�
		int currDigit = pow(10, exp);

		// arr ���� �� ���Ͽ� �з�
		for (int i = 0; i < size; ++i) {
			int radix = arr[i] / currDigit;
			radix %= 10;
			buckets[radix].push_back(arr[i]);
		}

		// ������ �ϳ��� ����
		for (const auto& bucket : buckets) {
			mergedBucket.insert(mergedBucket.end(), bucket.begin(), bucket.end());
		}

		// �迭�� ���ĵ� ���·� ����
		for (int i = 0; i < size; ++i) {
			arr[i] = mergedBucket[i];
		}

		// ���� ���� �ʱ�ȭ
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

	// ������ �۾� ���� ����
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

	// �ִ� �ڸ����� ã�� ����, ���� �迭�� �ִ� ã��
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

	// �ִ��� �ڸ��� ã��
	int maxDigit = 0;
	for (int i = 1; (max - i) > 0; i *= 10) {
		maxDigit++;
	}

	// �� �ڸ����� Bucketing ����
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

	// ���� Radix Sorting �� ����� ���� Radix Sorting �� ����� ���ϱ� ����,
	// ������ arr �迭�� temp �迭�� �����صΰ�,
	// ���� Radix Sorting �� ���� arr �迭�� ���ϸ� temp �迭�� ���� �ٽ� ������ ����.
	int* temp = new int[size];
	for (int i = 0; i < size; ++i)
		temp[i] = arr[i];

	// Sort arr !!

	// �������� Radix Sort
	SeqRadixSort(arr, size);
	
	// arr �ʱ�ȭ
	int* seqArr = new int[size];
	for (int i = 0; i < size; ++i) {
		seqArr[i] = arr[i];
		arr[i] = temp[i];
	}

	// �������� Radix Sort
	ParRadixSort(arr, size);

	std::cout << arr[size / 2];


	delete[] arr;
	delete[] temp;
	delete[] seqArr;
}