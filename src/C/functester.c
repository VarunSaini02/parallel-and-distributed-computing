#include <stdio.h>
#include <stdlib.h>
#include "func.h"

int main(int argc, char* argv[]) {
   pair_t* element;
   element = malloc(sizeof(pair_t));
   if (element == NULL) {
      printf("Error on malloc call");
      exit(-1);
   }
   element->first = 10;
   element->last = 20;
   int rc = func(element);
   if (rc != 0) {
      printf("There was an error on func call\n");
      exit(-1);
   }
   rc = func(element);
   if (rc != 0) {
      printf("There was an error on func call\n");
      exit(-1);
   }
   free(element);
   return 0;
}