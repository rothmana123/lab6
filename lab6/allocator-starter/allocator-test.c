#include <stdlib.h>
#include <stdio.h>

int main(void)
{
    int *allocs[6];
    for (int i = 0; i < 6; ++i) {
        allocs[i] = malloc(1 + i * 2);
    }
    for (int i = 0; i < 6; ++i) {
        free(allocs[i]);
    }
    int *z = malloc(1);
    int *y = calloc(2, 2);
    int *x = calloc(4, 3);
    *x = 99;
    
    x = realloc(x, 1);
    x = realloc(x, 1000);
    
    printf("X is: %d\n", *x);
    
    char *q = realloc(NULL, 2);
    
    free(z);
    free(y);
    free(x);
    free(q);
    
    return 0;
}