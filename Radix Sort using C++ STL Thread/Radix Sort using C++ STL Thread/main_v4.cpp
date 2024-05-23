#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <cmath>

class RadixVar {
private:
    int maxDigit;
	int radix;
    std::vector<int*> buckets;
    int* bucketSize;
    int* mergedBucket;

public:
	// 기본 생성자
    RadixVar() : maxDigit(0), radix(10), bucketSize(nullptr), mergedBucket(nullptr) {}
	
	void SetMaxDigit(int digit) {
		maxDigit = digit;
	}

	int GetMaxDigit() {
		return maxDigit;
	}

	int GetRadix() {
		return radix;
	}

	void AllocBucket(int* bucket) {
		buckets.push_back(bucket);
	}

	int* MergeBuckets(int size) {
		int* mergedBucket = new int[size];
		int mergeIndex = 0;

		for (int i = 0; i < 10; ++i) {
			std::copy(buckets[i], buckets[i] + bucketSize[i], mergedBucket + mergeIndex);
			mergeIndex += bucketSize[i];
		}

		return mergedBucket;
	}

	void Clear() {
		for (int i = 0; i < 10; i++)
			delete[] buckets[i];

		delete[] bucketSize;
		delete[] mergedBucket;
		buckets.clear();
	}

	~RadixVar() {
		Clear();
	}
};

class ThreadManager {
private:
	std::vector<std::thread> threads;

public:
	void CreateThread() {

	}
};

void SearchMaxDigit(int* arr, int size, RadixVar& radixVar) {

	int max = arr[0];

	// 전체 배열에서 최댓값 찾기
	for (int i = 1; i < size; ++i) {
		if (max < arr[i])
			max = arr[i];
	}
	
	int digit = 0;

	// 최댓값의 자릿수 값 찾기
	for (long long int radix = 1; (long long int)(max / radix) > 0; radix *= 10) {
		digit++;
	}

	// 최댓값의 자릿수 값 등록
	radixVar.SetMaxDigit(digit);
}

void RadixSort(RadixVar& radixVar) {

	for (int i = 0; i < radixVar.GetMaxDigit(); ++i) {

		long long int currDigit = pow(radixVar.GetRadix(), i);

		radixVar.SetBucketSize(i, )

		for (int i = 0; i < 10; ++i)
			bucketSize[i] = 0;

		// 병렬화
		for (int i = 0; i < size; ++i) {
			int num = arr[i] / currDigit;
			num %= 10;
			bucketSize[num]++;

	}
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
	RadixVar radixVar;
	SearchMaxDigit(arr, size, radixVar);
	RadixSort();

	std::cout << arr[size / 2];
}