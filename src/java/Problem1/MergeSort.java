import java.util.Collections;
import java.util.concurrent.ThreadLocalRandom;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

class MergeSort {

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

   static void mergeSortHelper(int arr[], int i, int len) {

      if (len == 1)
            return;

      int left_len = len/2;
      int right_len = len - left_len;

      Thread t1 = new Thread(new Runnable() {
            @Override
            public void run() {
                  mergeSortHelper(arr, i, left_len);
            }
      });      
      Thread t2 = new Thread(new Runnable() {
            @Override
            public void run() {
                  mergeSortHelper(arr, i+left_len, right_len);
            }
      });
      t1.start();
      t2.start();
      try {
         t1.join();
         t2.join();
      } catch (InterruptedException ex) {
      }

      merge(arr, i, i+left_len, left_len, right_len);
   }

   public static void mergeSort(int arr[]) {
      mergeSortHelper(arr, 0, arr.length);
   }

   public static void main(String args[]) {
      // // Test Merge
      // int min = 0;
      // int max = 100;
      // List<Integer> list1 = new ArrayList<>();
      // for (int i = 0; i < 100; i++) {
      //       int randomNum = ThreadLocalRandom.current().nextInt(min, max + 1);
      //       list1.add(randomNum);
      // }

      // Collections.sort(list1);

      // List<Integer> list2 = new ArrayList<>();
      // for (int i = 0; i < 100; i++) {
      //       int randomNum = ThreadLocalRandom.current().nextInt(min, max + 1);
      //       list2.add(randomNum);
      // }

      // Collections.sort(list2);

      // list1.addAll(list2);

      // int[] arr = list1.stream().mapToInt(i -> i).toArray();

      // merge(arr, 0, 100, 100, 100);

      // System.out.println(Arrays.toString(arr));

      // Test MergeSort
      List<Integer> list = new ArrayList<>();
      for (int i = 0; i < 200; i++) {
            list.add(i);
      }

      Collections.shuffle(list);

      int[] arr = list.stream().mapToInt(i -> i).toArray();

      System.out.println(Arrays.toString(arr));
      System.out.println();

      mergeSort(arr);

      System.out.println(Arrays.toString(arr));
   }
}