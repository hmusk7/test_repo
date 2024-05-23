#include <iostream>
#include <thread>
#include <cstdlib>

int main()
{
	srand(0);

	int size;
	std::cin >> size;
	int* arr = new int[size];
	for (int i = 0; i < size; i++)
		arr[i] = rand();

	// Sort arr !!

	std::cout << arr[size / 2];
}