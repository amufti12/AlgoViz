#ifndef SORTS_HPP_
#define SORTS_HPP_

#include "Utils.h"

#include <thread>

enum SortType
{
  SELECTION,
  BUBBLE,
  INSERTION,
  MERGE,
  QUICK
};

// Swapping indexes
void swap(SortableRenderData* a, SortableRenderData* b)
{
  SortableRenderData temp;
  temp = *a;
  *a = *b;
  *b = temp;
}
//////// Selection Sort - find minimum element in arr and place at beginning
// Time comp: O(n^2), Space Comp: O(1)
void selectionSort(SortableRenderData* arr, int size, std::function<void()>&& sleepFunc)
{
  int i, j, imin;
  for (i = 0; i < size - 1; i++) {
    {
      std::lock_guard<std::mutex> lock(sortMutex);
      imin = i; // getting the index of minimum data
      for (j = i + 1; j < size; j++) {
        if (arr[j] < arr[imin]) {
          imin = j;
          // swapping to the correct position
        }
      }
      arr[i].selected = true;
      swap(&arr[i], &arr[imin]);
    }
    sleepFunc();
  }
}
//////// Bubble Sort - swapping adjacent elements until sorted
void bubbleSort(SortableRenderData* arr, int size, std::function<void()>&& sleepFunc)
{
  for (int i = 0; i < size; i++) {
    // Detecting if there is a swap
    int swaps = 0;
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        arr[j].selected = true;
        swap(&arr[j], &arr[j + 1]);
        swaps = 1; // setting the swap flag
        sleepFunc();
      }
    }

    // If there are no swaps then the array is sorted
    if (!swaps) {
      break;
    }
  }
}
//////// Insertion Sort - Sorting an array of size n in ascending order:
// Iterate from arr[1] to arr[n] over the array
// Compare the current element (key) to its predecessor
// It the key element is smaller than its predecessor
// compare it to the elements before. Move the greater elements
// one positionup to make space for the swapped element
void insertionSort(SortableRenderData* arr, int size, std::function<void()>&& sleepFunc)
{
  SortableRenderData key;
  int j;
  for (int i = 1; i < size; i++) {
    key = arr[i]; // take value
    j = i;
    while (j > 0 && arr[j - 1] > key) {
      arr[j - 1].selected = true;
      arr[j] = arr[j - 1];
      j--;
      sleepFunc();
    }
    // inserting it in the right place
    arr[j] = key;
  }
}

//////// Merge Sort
/*
It there is only one element in list then it is sorted - return
Divide the list recursively into two halves until end
Merge the smaller lists into new list in sorted order
*/
// Merges two subarrays of arr[]. The left: arr[l..m] and the right: arr[m+1..r]
void merge(SortableRenderData* arr, int l, int m, int r, std::function<void()>&& sleepFunc)
{
  // Initializing nl and nr as the size of the left and right sub arrays
  int nl = m - l + 1;
  int nr = r - m;

  // Create temp arrays left and right
  SortableRenderData* Larr = new SortableRenderData[nl];
  SortableRenderData* Rarr = new SortableRenderData[nr];

  // Copy data to temp arrays Larr[] and Rarr[]
  for (int i = 0; i < nl; i++) {
    Larr[i] = arr[l + i];
  }
  for (int j = 0; j < nr; j++) {
    Rarr[j] = arr[m + 1 + j];
  }

  // Initial index of first, second, and third subarray
  int i = 0;
  int j = 0;
  int k = l;

  // Merge the temp arrays back into arr[l..r]
  while (i < nl && j < nr) {
    if (Larr[i] <= Rarr[j]) {
      Larr[i].selected = true;
      arr[k] = Larr[i];
      i++;
      sleepFunc();
    } else {
      Rarr[i].selected = true;
      arr[k] = Rarr[j];
      j++;
      sleepFunc();
    }
    k++;
  }

  // Copy the remaining elements of L[], if there are any
  while (i < nl) {
    arr[k] = Larr[i];
    i++;
    k++;
  }

  // Copy the remaining elements of R[], if there are any
  while (j < nr) {
    arr[k] = Rarr[j];
    j++;
    k++;
  }

  delete Larr;
  delete Rarr;
}

// l is for left index and r is right index of the sub-array of arr to be sorted
void mergeSort(SortableRenderData* arr, int l, int r, std::function<void()>&& sleepFunc)
{
  if (l >= r) {
    return; // returns recursively
  }
  int m = l + (r - l) / 2;
  // Sorts the first and second arrays then merges them
  mergeSort(arr, l, m, std::move(sleepFunc));
  mergeSort(arr, m + 1, r, std::move(sleepFunc));
  merge(arr, l, m, r, std::move(sleepFunc));
}

//////// Quick Sort
/*
- Choose the highest index value as pivot val
- Take two variables to point left and right of the list excluding pivot val
- Left points to the low index
- Right points to the  high index
- While value at left is less than pivot move right
- While value at right is greater than pivot move left
- If both set 5 and step 6 does not match swap left and right
- If left >= right, the point where they met is the new pivot
*/
// Partition function to split array based on values at high as pivot value
int Partition(SortableRenderData* arr, int low, int high, std::function<void()>&& /*sleepFunc*/)
{
  int pivot, index, i;
  index = low;
  pivot = high;
  for (i = low; i < high; i++) {
    // Find the index of the pivot
    if (arr[i] < arr[pivot]) {
      swap(&arr[i], &arr[index]);
      index++;
    }
  }
  swap(&arr[pivot], &arr[index]);
  return index;
}
// Randomly selecting our pivot
int RandomPivotSelection(SortableRenderData* arr, int low, int high, std::function<void()>&& sleepFunc)
{
  int pivot, n;
  n = rand();
  // Randomizing pivot value from the sub-array
  pivot = low + n % (high - low + 1);
  swap(&arr[high], &arr[pivot]);
  return Partition(arr, low, high, std::move(sleepFunc));
}
void quickSort(SortableRenderData* arr, int p, int q, std::function<void()>&& sleepFunc)
{
  // Recursively sorting list
  int pindex;
  if (p < q) {
    pindex = RandomPivotSelection(arr, p, q, std::move(sleepFunc));
    // Recursive QuickSort
    quickSort(arr, p, pindex - 1, std::move(sleepFunc));
    quickSort(arr, pindex + 1, q, std::move(sleepFunc));
  }
}

//////// Heap Sort

//////// Radix Sort

//////// Binary Insertion Sort

void Sort(SortableRenderData* array, int32_t size, SortType type, std::function<void()>&& sleepFunc)
{
  if (type == SortType::SELECTION) {
    selectionSort(array, int32_t(size), std::move(sleepFunc));
  } else if (type == SortType::BUBBLE) {
    bubbleSort(array, int32_t(size), std::move(sleepFunc));
  } else if (type == SortType::INSERTION) {
    insertionSort(array, int32_t(size), std::move(sleepFunc));
  } else if (type == SortType::MERGE) {
    mergeSort(array, 0, int32_t(size) - 1, std::move(sleepFunc));
  } else if (type == SortType::QUICK) {
    quickSort(array, 0, int32_t(size) - 1, std::move(sleepFunc));
  }
}

#endif