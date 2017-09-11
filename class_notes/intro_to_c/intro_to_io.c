#include <stdio.h>
#define TRUE 1

int main() {
    char ch;
    while(TRUE) {
        int okay = scanf("%c", &ch);
        printf("%c\n", ch);
    }
}
