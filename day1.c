#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ELVES 3
int elves[MAX_ELVES];

// This function is insert sorts the limited-size elves array
// to keep track of the elves carrying the top-MAX_ELVES number
// of calories.
void add_elf(int elf) {
    int t;
    for (int i = 0; i < MAX_ELVES; i++) {
        if (elf > elves[i]) {
            t = elves[i];
            elves[i] = elf;
            elf = t;
        }
    }
} 

// Just a helper function to sum the array of elves.
int elves_sum() {
    int tot = 0;
    for (int i = 0; i < MAX_ELVES; i++) {
        tot += elves[i];
    }
    return tot;
}

int main() {
    FILE* f = fopen("input/day1.txt", "r");
    memset(elves, 0, sizeof(int) * MAX_ELVES);

    char buf[100];
    int cals = 0;

    // Read the input file one line at a time. If we get to a blank
    // new line, that means we've reached the end of an elf and can
    // add the number of calories they're carrying to the elves array.
    // (Or not add that number, if they're carrying fewer than the lowest
    // amount carried in the top-MAX_ELVES elves);
    while (fgets(buf, 100, f)) {
        if (strcmp(buf, "\n")) {
            cals += atoi(buf);
        } else {
            add_elf(cals);
            cals = 0;
        }
    }

    printf("Part 1: %d\n", elves[0]);
    printf("Part 2: %d\n", elves_sum());
}