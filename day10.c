#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// For Part 1
static unsigned long result = 0;

void cycle(unsigned long* cycle_n, long regist) {
    // Each cycle we print one character. If cycle_n % 40 is within 1 of
    // the x register value, we print a # instead of a .
    if (abs(regist - *cycle_n % 40) <= 1) {
        printf("#");
    } else {
        printf(".");
    }

    *cycle_n = *cycle_n + 1;
    
    if ((*cycle_n) % 40 == 0) {
        printf("\n");
    }
    if (*cycle_n % 40 == 20)    {
        result += *cycle_n * regist; // part 1
    }
}

int main() {
    FILE *f = fopen("input/day10.txt", "r");
    if (errno) {
        printf("Could not open input/day10.txt: %d\n", errno);
        return errno;
    }

    char buf[20];
    long regist = 1;
    unsigned long cycle_n = 0;
    while(fgets(buf, 20, f)) {
        if (buf[0] == 'n') {
            cycle(&cycle_n, regist);
        }
        if (buf[0] == 'a') {
            cycle(&cycle_n, regist);
            cycle(&cycle_n, regist);
            regist += atoi(&buf[5]);
        }
    }

    printf("Part 1: %lu\n", result);
    printf("Part 2 is already printed by cycle().\n");
}