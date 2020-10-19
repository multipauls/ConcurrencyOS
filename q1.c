#define _POSIX_C_SOURCE 199309L //required for clock
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <inttypes.h>
#include <math.h>

void selSort(int arr[], int l, int r)  
{  
    int i, j, min_idx;  
  
    for (i = l; i <=r; i++)  
    {  
        min_idx = i;  
        for (j = i+1; j <=r; j++)  
        if (arr[j] < arr[min_idx])  
            min_idx = j;  
  
        int temp;
        temp=arr[min_idx];
        arr[min_idx]=arr[i];
        arr[i]=temp;  
    }  
}  

int * shareMem(size_t size){
     key_t mem_key = IPC_PRIVATE;
     int shm_id = shmget(mem_key, size, IPC_CREAT | 0666);
     return (int*)shmat(shm_id, NULL, 0);
}

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    int L[n1], R[n2]; 
  
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    k = l; 
    i = 0; 
    j = 0;  
     
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
     while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    }
   
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
       
} 



void conMergeSort(int arr[], int l, int r) 
{ 
    if (l < r) { 
        
        int m = l + (r - l) / 2; 
             int lpid=fork();
             if (lpid<0){
               perror("Error in forking left child");
               exit(1);
             }
             if (lpid==0){
               //if(m-l>5)
                       conMergeSort(arr, l, m);
                  //else
                  //     selSort(arr, l, m);
                  exit(0);
               }
               else{
                    int rpid=fork(); 
                    if (rpid<0){
                         perror("Error in forking right child");
                         exit(1);
                    }
                    if (rpid==0){
                    //   if (r-m-1>5)
                         conMergeSort(arr, m + 1, r);
                   //    else
                   //         selSort(arr, m + 1, r);
                    exit(0);
                   }
                   else{
                    int status1, status2;
                         waitpid(lpid, &status1, 0);
                         waitpid(rpid, &status2, 0);
                         //for(int i=l; i<=r; i++){
                         //          printf("%d ", arr[i]);
                                   
                         //}
                         //printf("\n");
                         merge(arr, l, m, r);
                         //for(int i=l; i<=r; i++){
                         //          printf("%d ", arr[i]);
                                   
                         //}
                         //printf("\n");
                    } 
               }
               merge(arr, l, m, r);
         
    } 
} 

void mergeSort(int arr[], int l, int r)  
{  
    if (l < r) {  
        int m = l + (r - l) / 2;  
        mergeSort(arr, l, m);  
        mergeSort(arr, m + 1, r);  
  
        merge(arr, l, m, r);  
    }  
}  



void runSorts(long long int n){

     struct timespec ts;

     //getting shared memory
     int *arr = shareMem(sizeof(int)*(n+1));
     for(int i=0;i<n;i++) scanf("%d", arr+i);

     int brr[n+1];
     for(int i=0;i<n;i++) brr[i] = arr[i];

     printf("Running concurrent_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort
     conMergeSort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     long double en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t1 = en-st;

     printf("Running normal_mergesort for n = %lld\n", n);
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     st = ts.tv_nsec/(1e9)+ts.tv_sec;

     //multiprocess mergesort
     mergeSort(arr, 0, n-1);
     for(int i=0; i<n; i++){
          printf("%d ",arr[i]);
     }
     printf("\n");
     clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
     en = ts.tv_nsec/(1e9)+ts.tv_sec;
     printf("time = %Lf\n", en - st);
     long double t2 = en-st;


     

     printf("normal_quicksort ran:\n\t[ %Lf ] times faster than concurrent_quicksort\n\t", t1/t2);
     shmdt(arr);
     return;
}

int main(){

     long long int n;
     scanf("%lld", &n);
     runSorts(n);
     return 0;
}
