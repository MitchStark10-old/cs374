/* main.c
...
*/

#include <stdio.h> //printf, scanf

void swap(int * first, int * second) {
    int temp = *first;
    *first = *second;
    *second = temp;
}

int main() {
    int w = 11;
    int x = 22;
    int y = 33;
    int z = 44;
    printf("Before: w = %d, x = %d, y = %d, z = %d", w, x, y, z);
    swap(&w, &x);
    swap(&y, &z);
    printf("\n\nAfter: w = %d, x = %d, y = %d, z = %d\n", w, x, y, z);
}
