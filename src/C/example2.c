#include <stdio.h>
#include <stdlib.h>

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
   pair_t* element;
   element = malloc(sizeof(pair_t));
   element->first = 10;
   element->last = 20;
   func(element);
   func(element);
   free(element);
   return 0;
}