#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//The definition of group_size for elves. Didn't want to magic-number it too much.
#define GROUP_SIZE 3

//Converts an ascii [A-Za-z] value into the scoring used in the problem. 
int item_value(int item) {
    if (item >= 'A' && item <= 'Z') {
        return item - 'A' + 27;
    }
    if (item >= 'a' && item <= 'z') {
        return item - 'a' + 1;
    }
    if (item == '\n') {
        return 0;
    }
    printf("item_value: Invalid character: %c, ascii val %d\n", item, item);
    return 0;
}

// The function for part 1. We go through the first half of each line and mark
// each character we see as seen. Then we go through the second half of each
// line and, as soon as we see a character we've already seen, that's the dup.
int get_duplicate_item(char* buf, int length) {
    int* seen = calloc(sizeof(int), 53);
    
    for(int i = 0; i < length / 2; i++) {
        seen[item_value(buf[i])] = true;
    }

    for(int i = (length / 2); i < length; i++) {
        if (seen[item_value(buf[i])]) {
            free(seen);
            return item_value(buf[i]);
        }
    }
    
    printf("get_duplicate_item: No duplicate found. Buffer: %s\n", buf);
    free(seen);
    return 0;
}

// The function for part 2. It takes a char** of length GROUP_SIZE. It
// then creates GROUP_SIZE number of arrays to track which characters
// have been seen across each line. It then goes through each line and
// does that tracking. Finally, it goes through and counts up how many
// times each character has been seen across the three lines. If it's
// three times, that's the badge ID.
int get_group_item(char** bufs) {
    int** seen = malloc(sizeof(int*) * GROUP_SIZE);
    for (int i = 0; i < GROUP_SIZE; i++) {
        seen[i] = calloc(sizeof(int), 53);

    }

    for (int i = 0; i < GROUP_SIZE; i++) {
        int buflen = strlen(bufs[i]);
        for (int j = 0; j < buflen; j++) {
            seen[i][item_value(bufs[i][j])] = 1;
        }
    }

    for (int i = 0; i < 53; i++) {
        int tot_seen = 0;
        for (int j = 0; j < GROUP_SIZE; j++) {
            tot_seen += seen[j][i];
        }
        
        if (tot_seen == 3) {
            for (int i = 0; i < GROUP_SIZE; i++) {
                free(seen[i]);
            }
            free(seen);
            return i;
        }
    }
    printf("get_group_item: No duplicate found. Buffer 1: %s\n", bufs[0]);
    return 0;
}

// Should be easy to follow. Open the file, allocate some space to
// track the inputs, and then handle the inputs.
int main() {
    FILE* f = fopen("input/day3.txt", "r");
    if (errno) {
        printf("Couldn't open input/day3.txt: %d\n", errno);
        return errno;
    }

    ssize_t buflen = 100;
    char* buf = malloc(sizeof(char) * buflen);
    char** bufs = malloc(sizeof(char*) * GROUP_SIZE);

    int p1_total = 0;
    int p2_total = 0;
    int lines_counted = 0;
    while(getline(&buf, &buflen, f) > 0) {
        buflen = strlen(buf);
        if (buf[buflen - 1] == '\n') {
            buf[buflen - 1] = '\0';
            buflen--;
        }

        //part 1:
        p1_total += get_duplicate_item(buf, buflen);

        //part 2:
        char* bufcpy = malloc(sizeof(char) * buflen + 1);
        bufs[lines_counted++] = strcpy(bufcpy, buf);
        if (lines_counted == GROUP_SIZE) {
            p2_total += get_group_item(bufs);
            lines_counted = 0;
            for(int i = 0; i < GROUP_SIZE; i++) {
                free(bufs[i]);
            }
        }
    }

    free(buf);
    free(bufs);

    printf("P1 Total: %d\n", p1_total);
    printf("P2 Total: %d\n", p2_total);
}