#include "func.h"
#include <stdio.h>

int func(pair_t *element){
    printf("Data is %d and %d\n", element->first, element->last);
    element->first += 1;
    element->last -= 1;
    return 0;
}