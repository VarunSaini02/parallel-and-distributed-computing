#include <stdio.h>

typedef struct pair_st {
   int first;
   int last;
} pair_t;

void func(pair_t* element) {
   printf("Data is %d and %d\n", element->first, element->last);
   element->first+=1;
   element->last-=1;
}

int main(int argc, char* argv[]) {
   pair_t element;
   element.first = 10;
   element.last = 20;
   printf("The value of element is %p\n", &element);
   int* fptr = (int*) &element;
   printf("The value is %d\n", *fptr);
   fptr++;
   printf("The value is %d\n", *fptr);
   fptr++;

   func(&element);
   func(&element);
   return 0;
}