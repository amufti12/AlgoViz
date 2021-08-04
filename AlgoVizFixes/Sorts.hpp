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

// loops over sortable render data and declares them false
void resetColorData(std::vector<SortableRenderData>& arr)
{
  for (auto& srd : arr) {
    srd.high = false;
    srd.low = false;
    srd.selected = false;
  }
}

//////// Selection Sort - find minimum element in arr and place at beginning
// Time comp: O(n^2), Space Comp: O(1)
void selectionSort(std::vector<SortableRenderData>& arr, int size, std::shared_future<void>& f)
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

    if (f.wait_for(1ms) != std::future_status::timeout) {
      return;
    } else {
      std::chrono::duration<double> d;
      {
        std::lock_guard<std::mutex> lock3(updateFreqMutex);
        d = 1000ms / numUpdatesPerSec;
      }
      std::this_thread::sleep_for(d);
      {
        std::lock_guard<std::mutex> lock(sortMutex);
        resetColorData(arr);
      }
    }
  }
}
//////// Bubble Sort - swapping adjacent elements until sorted
void bubbleSort(std::vector<SortableRenderData>& arr, int size, std::shared_future<void>& f)
{
  for (int i = 0; i < size; i++) {
    // Detecting if there is a swap
    int swaps = 0;
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        {
          std::lock_guard<std::mutex> lock(sortMutex);
          arr[j].selected = true;
          swap(&arr[j], &arr[j + 1]);
          swaps = 1; // setting the swap flag
        }

        if (f.wait_for(1ms) != std::future_status::timeout) {
          return;
        } else {
          std::chrono::duration<double> d;
          {
            std::lock_guard<std::mutex> lock3(updateFreqMutex);
            d = 1000ms / numUpdatesPerSec;
          }
          std::this_thread::sleep_for(d);
          {
            std::lock_guard<std::mutex> lock(sortMutex);
            resetColorData(arr);
          }
        }
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
void insertionSort(std::vector<SortableRenderData>& arr, int size, std::shared_future<void>& f)
{
  SortableRenderData key;
  int j;
  for (int i = 1; i < size; i++) {
    {
      std::lock_guard<std::mutex> lock(sortMutex);
      key = arr[i]; // take value
    }
    j = i;
    while (j > 0 && arr[j - 1] > key) {
      {
        std::lock_guard<std::mutex> lock(sortMutex);
        arr[j - 1].selected = true;
        arr[j] = arr[j - 1];
        j--;
      }

      if (f.wait_for(1ms) != std::future_status::timeout) {
        return;
      } else {
        std::chrono::duration<double> d;
        {
          std::lock_guard<std::mutex> lock3(updateFreqMutex);
          d = 1000ms / numUpdatesPerSec;
        }
        std::this_thread::sleep_for(d);
        {
          std::lock_guard<std::mutex> lock(sortMutex);
          resetColorData(arr);
        }
      }
    }
    // inserting it in the right place
    {
      std::lock_guard<std::mutex> lock(sortMutex);
      arr[j] = key;
    }
  }
}

//////// Merge Sort
/*
It there is only one element in list then it is sorted - return
Divide the list recursively into two halves until end
Merge the smaller lists into new list in sorted order
*/
// Merges two subarrays of arr[]. The left: arr[l..m] and the right: arr[m+1..r]
void merge(std::vector<SortableRenderData>& arr, int l, int m, int r, std::shared_future<void>& f)
{
  // Initializing nl and nr as the size of the left and right sub arrays
  int nl = m - l + 1;
  int nr = r - m;

  // Create temp arrays left and right
  std::vector<SortableRenderData> Larr(nl);
  std::vector<SortableRenderData> Rarr(nr);

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
      {
        std::lock_guard<std::mutex> lock(sortMutex);
        Larr[i].selected = true;
        arr[k] = Larr[i];
        i++;
      }

      if (f.wait_for(1ms) != std::future_status::timeout) {
        return;
      } else {
        std::chrono::duration<double> d;
        {
          std::lock_guard<std::mutex> lock3(updateFreqMutex);
          d = 1000ms / numUpdatesPerSec;
        }
        std::this_thread::sleep_for(d);
        {
          std::lock_guard<std::mutex> lock(sortMutex);
          resetColorData(arr);
        }
      }
    } else {
      {
        std::lock_guard<std::mutex> lock(sortMutex);
        Rarr[j].selected = true;
        arr[k] = Rarr[j];
        j++;
      }

      if (f.wait_for(1ms) != std::future_status::timeout) {
        return;
      } else {
        std::chrono::duration<double> d;
        {
          std::lock_guard<std::mutex> lock3(updateFreqMutex);
          d = 1000ms / numUpdatesPerSec;
        }
        std::this_thread::sleep_for(d);
        {
          std::lock_guard<std::mutex> lock(sortMutex);
          resetColorData(arr);
        }
      }
    }
    k++;
  }

  // Copy the remaining elements of L[], if there are any
  while (i < nl) {
    {
      std::lock_guard<std::mutex> lock(sortMutex);
      arr[k] = Larr[i];
      i++;
      k++;
    }
  }

  // Copy the remaining elements of R[], if there are any
  while (j < nr) {
    {
      std::lock_guard<std::mutex> lock(sortMutex);
      arr[k] = Rarr[j];
      j++;
      k++;
    }
  }
}

// l is for left index and r is right index of the sub-array of arr to be sorted
void mergeSort(std::vector<SortableRenderData>& arr, int l, int r, std::shared_future<void>& f)
{
  if (l >= r) {
    return; // returns recursively
  }
  int m = l + (r - l) / 2;
  // Sorts the first and second arrays then merges them
  mergeSort(arr, l, m, f);
  mergeSort(arr, m + 1, r, f);
  merge(arr, l, m, r, f);
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
int Partition(std::vector<SortableRenderData>& arr,
              int low,
              int high,
              std::shared_future<void>& f,
              bool& pShouldExit)
{
  int pivot, index, i;
  index = low;
  pivot = high;

  // Show the high value -------------------------------------------------------------------------------------
  for (i = low; i < high; i++) {
    if (f.wait_for(1ms) != std::future_status::timeout) {
      pShouldExit = true;
      return -1;
    }

    // Find the index of the pivot
    // Show the pivot and high index values
    // -------------------------------------------------------------------------------------
    if (arr[i] < arr[pivot]) {
      swap(&arr[i], &arr[index]);
      index++;
    }

    if (f.wait_for(1ms) != std::future_status::timeout) {
      pShouldExit = true;
      return -1;
    } else {
      std::chrono::duration<double> d;
      {
        std::lock_guard<std::mutex> lock3(updateFreqMutex);
        d = 1000ms / numUpdatesPerSec;
      }
      std::this_thread::sleep_for(d);
      {
        std::lock_guard<std::mutex> lock(sortMutex);
        resetColorData(arr);
      }
    }

    std::lock_guard<std::mutex> lock(sortMutex);
    arr[pivot].selected = true;
    arr[index].high = true;
    arr[low].low = true;
  }

  {
    std::lock_guard<std::mutex> lock(sortMutex);
    swap(&arr[pivot], &arr[index]);
    return index;
  }
}

// Randomly selecting our pivot
int RandomPivotSelection(std::vector<SortableRenderData>& arr,
                         int low,
                         int high,
                         std::shared_future<void>& f,
                         bool& pShouldExit)
{
  int pivot, n;
  n = rand();
  // Randomizing pivot value from the sub-array
  pivot = low + n % (high - low + 1);

  {
    std::lock_guard<std::mutex> lock(sortMutex);
    swap(&arr[high], &arr[pivot]);
  }

  bool shouldExit = false;
  int ret = Partition(arr, low, high, f, shouldExit);
  if (shouldExit) {
    pShouldExit = true;
    return -1;
  }
  return ret;
}
void quickSort(std::vector<SortableRenderData>& arr, int low, int high, std::shared_future<void>& f)
{
  // Recursively sorting list
  int pindex;
  if (low < high) {
    bool shouldExit = false;
    pindex = RandomPivotSelection(arr, p, q, f, shouldExit);

    if (shouldExit) {
      return;
    }
    // Recursive QuickSort
    quickSort(arr, low, pindex - 1, f);
    quickSort(arr, pindex + 1, high, f);
  }
}

//////// Heap Sort

//////// Radix Sort

//////// Binary Insertion Sort

void Sort(std::vector<SortableRenderData>& array, SortType type, std::shared_future<void>& f)
{
  {
    std::lock_guard<std::mutex> lock(dataSizeMutex);
    activeDataSize = static_cast<int32_t>(array.size());
  }

  if (type == SortType::SELECTION) {
    selectionSort(array, int32_t(array.size()), f);
  } else if (type == SortType::BUBBLE) {
    bubbleSort(array, int32_t(array.size()), f);
  } else if (type == SortType::INSERTION) {
    insertionSort(array, int32_t(array.size()), f);
  } else if (type == SortType::MERGE) {
    mergeSort(array, 0, int32_t(array.size()) - 1, f);
  } else if (type == SortType::QUICK) {
    quickSort(array, 0, int32_t(array.size()) - 1, f);
  }
}

#endif