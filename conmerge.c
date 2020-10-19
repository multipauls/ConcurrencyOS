#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <sys/wait.h> 
#include <unistd.h> 


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
  

void mergeSort(int arr[], int l, int r) 
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
			        mergeSort(arr, l, m);
		        //else
		        //	selSort(arr, l, m);
		        exit(0);
			}
			else{
	        	int rpid=fork(); 
	        	if (rpid<0){
	        		perror("Error in forking right child");
	        		exit(1);
	        	}
	        	if (rpid==0){
	        	//	if (r-m-1>5)
		    	    	mergeSort(arr, m + 1, r);
	    	    //	else
	    	    //		selSort(arr, m + 1, r);
	    	    	exit(0);
	    	    }
	    	    else{
	    	    	int status1, status2;
                    waitpid(lpid, &status1, 0);
                    waitpid(rpid, &status2, 0);
                    for(int i=l; i<=r; i++){
                    		printf("%d ", arr[i]);
                    		
                    }
                    printf("\n");
                    merge(arr, l, m, r);
                    for(int i=l; i<=r; i++){
                    		printf("%d ", arr[i]);
                    		
                    }
                    printf("\n");
	    	    } 
  			}
  			merge(arr, l, m, r);
    	    
    } 
} 


int main(){
	int n, var;
	scanf("%d", &n);
	int arr[1000];
	for(int i=0; i<n; i++){
		scanf("%d", &var);
		arr[i]=var;
	}
	mergeSort(arr, 0, n-1 );
	for(int i=0; i<n; i++){
		printf("%d ", arr[i]);
		
	}
	return 0;
}