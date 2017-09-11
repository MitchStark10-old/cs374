#include <stdio.h>

int main() {
    float   gpa;
    int     credits;
    char    gender;
    printf("\nEnter gpa, credits, and gender: ");
    scanf(" %f %d %c", &gpa, &credits, &gender);
    printf("\nGPA: %f, credits: %d, gender: %c\n", gpa, credits, gender);
}
