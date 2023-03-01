import java.util.Collections;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveAction;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

class MergeSort {

    public static class MergeSortTask extends RecursiveAction {

        private final int arr[];
        private final int i;
        private final int len;
  
        public MergeSortTask(int arr[], int i, int len) {
            this.arr = arr;
            this.i = i;
            this.len = len;
        }
  
        @Override
        protected void compute() {
            if (len == 1)
            return;

            int left_len = len/2;
            int right_len = len - left_len;

            invokeAll(
                new MergeSortTask(arr, i, left_len),
                new MergeSortTask(arr, i+left_len, right_len)
            );

            merge(arr, i, i+left_len, left_len, right_len);
        }
    }

    static void merge(int arr[], int i, int j, int left_len, int right_len) {
 
       int k = i;
       int m = j;
       List<Integer> result = new ArrayList<>();
 
 
       while ((k-i<left_len) && (m-j<right_len)) {
             if (arr[k] < arr[m]) {
                result.add(arr[k]);
                k+=1;
             } else {
                result.add(arr[m]);
                m+=1;
             }
       }
 
       while (k-i<left_len) {
             result.add(arr[k]);
             k+=1;
       }
 
       // Here as a comment to show completeness, but
       // not needed. Why?
       // while (m-j<right_len) {
       //     result.add(arr[m]);
       //     m+=1;
       // }
       // Instead, we set the right_len to m-j to avoid
       // copying unnecessary elements below.
       right_len = m-j;
 
 
       // Copy back
       for (k = 0; k < left_len + right_len; k++)
             arr[i+k] = result.get(k);
    }
 
    public static void mergeSort(int arr[]) {
        MergeSortTask mergeSortTask =
                  new MergeSortTask(arr, 0, arr.length);
        ForkJoinPool.commonPool().invoke(mergeSortTask);
    }
 
    public static void main(String args[]) {
       // Test MergeSort
       List<Integer> list = new ArrayList<>();
       for (int i = 0; i < 100000; i++) {
             list.add(i);
       }
 
       Collections.shuffle(list);
 
       int[] arr = list.stream().mapToInt(i -> i).toArray();
 
       System.out.println(Arrays.toString(arr));
 
       mergeSort(arr);
 
       System.out.println(Arrays.toString(arr));
    }
 }