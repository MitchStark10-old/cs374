#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 16

int main() {
    char staticArray[LENGTH] = "12345";
    int correspondingInt = 0;
    sscanf(staticArray, "%d", &correspondingInt);
    printf("corresponding int value: %d\n", correspondingInt);

    char* dynamicArray = malloc(LENGTH * sizeof(char));
    float floatVal = 3.1415926;
    sprintf(dynamicArray, "PI = %f\n", floatVal);
    printf("%s", dynamicArray);

    free(dynamicArray);
}
