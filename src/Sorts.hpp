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
//////// Selection Sort - find minimum element in arr and place at beginning
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
//////// Bubble Sort - swapping adjacent elements until sorted
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
//////// Insertion Sort - Sorting an array of size n in ascending order:
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

//////// Merge Sort
/*
It there is only one element in list then it is sorted - return
Divide the list recursively into two halves until end
Merge the smaller lists into new list in sorted order
*/
// Merges two subarrays of arr[]. The left: arr[l..m] and the right: arr[m+1..r]
void merge(int* arr, int l, int m, int r)
{
  // Initializing nl and nr as the size of the left and right sub arrays
  int nl = m - l + 1;
  int nr = r - m;

  // Create temp arrays left and right
  int* Larr = new int[nl];
  int* Rarr = new int[nr];

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
      arr[k] = Larr[i];
      i++;
    } else {
      arr[k] = Rarr[j];
      j++;
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
void mergeSort(int* arr, int l, int r)
{
  if (l >= r) {
    return; // returns recursively
  }
  int m = l + (r - l) / 2;
  // Sorts the first and second arrays then merges them
  mergeSort(arr, l, m);
  mergeSort(arr, m + 1, r);
  merge(arr, l, m, r);
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
int Partition(int* arr, int low, int high)
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
int RandomPivotSelection(int* arr, int low, int high)
{
  int pivot, n;
  n = rand();
  // Randomizing pivot value from the sub-array
  pivot = low + n % (high - low + 1);
  swap(&arr[high], &arr[pivot]);
  return Partition(arr, low, high);
}
void quickSort(int* arr, int p, int q)
{
  // Recursively sorting list
  int pindex;
  if (p < q) {
    pindex = RandomPivotSelection(arr, p, q);
    // Recursive QuickSort
    quickSort(arr, p, pindex - 1);
    quickSort(arr, pindex + 1, q);
  }
}

//////// Heap Sort

//////// Radix Sort

//////// Binary Insertion Sort
#endif