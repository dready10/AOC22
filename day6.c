#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Easy part 1. I assumed part 2 would be part 1 but
// more than four characters but still refused to
// pre-generalize. So two functions instead of one
// with an additional argument.
int get_marker(char *buf, int loop_effect) {
    int len = strlen(buf);
    for(int i = 3; i < len; i++) {
        if (buf[i-3] != buf[i-2] &&
            buf[i-2] != buf[i-1] &&
            buf[i-1] != buf[i] &&
            buf[i-3] != buf[i-1] &&
            buf[i-3] != buf[i] &&
            buf[i-2] != buf[i])
            return i + 1 + loop_effect;
    }
    return 0;
}

// Part 2 function. For each character, look at the
// previous 14 characters. If they all differ, done.
int get_message(char *buf, int loop_effect) {
    int len = strlen(buf);
    char *seen = malloc(sizeof(bool) * 26);
    bool bflag = false;
    for(int i = 13; i < len; i++) {
        memset(seen, 0, sizeof(bool) * 26);
        bflag = true;
        for (int j = i - 13; j <= i; j++) {
            if (seen[buf[j] - 'a']) {
                bflag = false;
                break;
            }
            seen[buf[j] - 'a'] =  true;
        }
        if (bflag) {
            return i + 1 + loop_effect;
        }
    }
    return 0;
}

int main() {
    FILE *f = fopen("input/day6.txt", "r");
    if (errno) {
        printf("Couldn't open file input/day6.txt: %d\n", errno);
        return errno;
    }

    char buf[256];
    int marker, message;
    int loop_effect = 0;
    while(fgets(buf, 255, f)) {
        printf("%s\n", buf);
        marker = (marker ? marker : get_marker(buf, loop_effect));
        message = get_message(buf, loop_effect);
        if (marker && message) {
            break;
        }
        fseek(f, -14, SEEK_CUR);
        loop_effect += 240;
    }
    printf("Marker at: %d\n", marker);
    printf("Message at: %d\n", message);
}