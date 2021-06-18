#ifndef SORTS_HPP_
#define SORTS_HPP_

// Swapping indexes
void swap(int* a, int* b)
{
  int temp;
  temp = *a;
  *a = *b;
  *b = temp;
}
// Selection Sort - find minimum element in arr and place at beginning
// Time comp: O(n^2), Space Comp: O(1)
void selectionSort(int* arr, int size)
{
  int i, j, imin;
  for (i = 0; i < size - 1; i++) {
    imin = i; // getting the index of minimum data
    for (j = i + 1; j < size; j++) {
      if (arr[j] < arr[imin]) {
        imin = j;
        // swapping to the correct position
      }
    }
    swap(&arr[i], &arr[imin]);
  }
}
// Bubble Sort - swapping adjacent elements until sorted
void bubbleSort(int* arr, int size)
{
  for (int i = 0; i < size; i++) {
    // Detecting if there is a swap
    int swaps = 0;
    for (int j = 0; j < size - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        swap(&arr[j], &arr[j + 1]);
        swaps = 1; // setting the swap flag
      }
    }
    // If there are no swaps then the array is sorted
    if (!swaps) {
      break;
    }
  }
}
// Insertion Sort - Sorting an array of size n in ascending order:
// Iterate from arr[1] to arr[n] over the array
// Compare the current element (key) to its predecessor
// It the key element is smaller than its predecessor
// compare it to the elements before. Move the greater elements
// one positionup to make space for the swapped element
void insertionSort(int* arr, int size)
{
  int key, j;
  for (int i = 1; i < size; i++) {
    key = arr[i]; // take value
    j = i;
    while (j > 0 && arr[j - 1] > key) {
      arr[j] = arr[j - 1];
      j--;
    }
    // inserting it in the right place
    arr[j] = key;
  }
}

// Merge Sort
// It there is only one element in list then it is sorted - return
// Divide the list recursively into two halves until end
// Merge the smaller lists into new list in sorted order
//////// (as seen on GfG - still personally going through it to understand it)
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
  int n1 = m - l + 1;
  int n2 = r - m;

  // Create temp arrays
  int L[n1], R[n2];

  // Copy data to temp arrays L[] and R[]
  for (int i = 0; i < n1; i++)
    L[i] = arr[l + i];
  for (int j = 0; j < n2; j++)
    R[j] = arr[m + 1 + j];

  // Merge the temp arrays back into arr[l..r]

  // Initial index of first subarray
  int i = 0;

  // Initial index of second subarray
  int j = 0;

  // Initial index of merged subarray
  int k = l;

  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  // Copy the remaining elements of
  // L[], if there are any
  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  // Copy the remaining elements of
  // R[], if there are any
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

// l is for left index and r is
// right index of the sub-array
// of arr to be sorted */
void mergeSort(int arr[], int l, int r)
{
  if (l >= r) {
    return; // returns recursively
  }
  int m = l + (r - l) / 2;
  mergeSort(arr, l, m);
  mergeSort(arr, m + 1, r);
  merge(arr, l, m, r);
}

// Quick Sort

// Heap Sort

// Radix Sort

// Binary Insertion Sort
#endif