#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void get_sections(char* buf, int* sections) {
    char* left;
    char* right;
    left = strtok(buf, ",");
    right = strtok(NULL, "\n");
    sections[0] = atoi(strtok(left, "-"));
    sections[1] = atoi(strtok(NULL, "-"));
    sections[2] = atoi(strtok(right, "-"));
    sections[3] = atoi(strtok(NULL, "-"));

    if (sections[3] - sections[2] < sections[1] - sections[0]) {
        int t = sections[0];
        sections[0] = sections[2];
        sections[2] = t;

        t = sections[1];
        sections[1] = sections[3];
        sections[3] = t;
    }
};

int main() {
    FILE* f = fopen("input/day4.txt", "r");
    if (errno) {
        printf("Couldn't open file input/day4.txt: %d\n", errno);
        return errno;
    }

    char buf[16];
    int sections[4];
    int subsumed = 0;
    int overlapping = 0;
    while (fgets(buf, 16, f)) {
        get_sections(buf, sections); //returns shorter section in [0]-[1]
        if (sections[1] <= sections[3] && sections[0] >= sections[2]) {
            subsumed++;
            overlapping++;
        } else if (sections[1] >= sections[2] && sections[0] <= sections[3]) {
            overlapping++;
        }
    }
    
    printf("Total subsumed: %d\n", subsumed);
    printf("Total overlapping: %d\n", overlapping);
}