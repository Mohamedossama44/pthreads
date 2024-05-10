#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <fstream>

using namespace std;
// Struct (arr-L-R)
struct ThreadArgs {
    vector<int>& arr;
    int left;
    int right;
};
//merge 2 sorted subarrays
void merge(vector<int>& arr, int left, int middle, int right) {
    //sizes of subarrays
    int s1 = middle - left + 1;
    int s2 = right - middle;
    //L & R hold the elements of subarrays
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];
    //i,j for subarrays k for original array
    int i = 0, j = 0, k = left;
    while (i < s1 && j < s2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < s1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < s2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}
// Function to perform merge sort on a subarray
void* mergeSort(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    int left = args->left;
    int right = args->right;
    if (left < right) {//subarray has more than one element
        int middle = left + (right - left) / 2;

        // Create threads for left and right halves
        pthread_t leftThread, rightThread;

        ThreadArgs leftArgs{args->arr, left, middle};
        ThreadArgs rightArgs{args->arr, middle + 1, right};

        pthread_create(&leftThread, nullptr, mergeSort, static_cast<void*>(&leftArgs));
        pthread_create(&rightThread, nullptr, mergeSort, static_cast<void*>(&rightArgs));

        // Wait for the threads to finish
        pthread_join(leftThread, nullptr);
        pthread_join(rightThread, nullptr);

        // Merge the sorted halves
        merge(args->arr, left, middle, right);
    }

    pthread_exit(nullptr);
}

// Function to perform parallel merge sort
void parallelMergeSort(vector<int>& arr) {
    int left = 0;
    int right = arr.size() - 1;
    ThreadArgs args{arr, left, right};
    pthread_t initialThread;
    pthread_create(&initialThread, NULL, mergeSort, &args);
    // Wait for the initial thread to finish
    pthread_join(initialThread, NULL);
}

int main() {
    // Input filename
    string filename;
    cout << "Enter the filename: ";
    cin >> filename;
    // Open the file
    ifstream inputFile1(filename);

    int num_elements;

    if (!inputFile1.is_open() ) {
        cerr << "Error opening file." << endl;
        return 1;
    }
    // Read size
    inputFile1 >> num_elements;
    //create arr with size 
    vector<int> arr(num_elements);
    //read arr
    for (int i=0;i<num_elements;i++){
        inputFile1 >> arr[i];
    }
    // Perform parallel merge sort
    parallelMergeSort(arr);

    // Print the sorted array
    cout << "\nSorted array:\n ";
    for (int num : arr) {
        cout << num << " ";
    }
    cout << endl;

    return 0;
}
