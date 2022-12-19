#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Mark a map with # from (oldx, oldy) > (x, y)
void record(char **map, int oldx, int oldy, int x, int y) {
    if (oldx == x) {
        int miny = (oldy < y ? oldy : y);
        int maxy = (oldy > y ? oldy : y);
        for (int i = miny; i <= maxy; i++) {
            map[i][x] = '#';
        }
    } else {
        int minx = (oldx < x ? oldx : x);
        int maxx = (oldx > x ? oldx : x);
        for (int i = minx; i <= maxx; i++) {
            map[y][i] = '#';
        }
    }
}

// Drop a piece of sand at (500,0) until we can't drop sand anymore.
// Per the problem, if the next "step" is a # or o, first check
// left, then check right. If can't move, rest.
int drop_sand(char **map, unsigned short xmin, unsigned short ymax) {
    int x = 500 - xmin;

    if (map[0][x] != '.') return 0;
    for (int i = 0; i <= ymax; i++) {
        if (map[i + 1][x] == '.') continue;
        if (map[i + 1][x - 1] == '.') {
            x -= 1;
            continue;
        }
        if (map[i + 1][x + 1] == '.') {
            x += 1;
            continue;
        }
        map[i][x] = 'o';
        return 1;
    }
    return 0;
}

int main() {
    FILE *f = fopen("input/day14.txt", "r");
    if (errno) {
        printf("Couldn't open input/day14.txt: %d\n", errno);
        return errno;
    }

    // First we need to get the maximum height of the puzzle.
    char buf[1024];
    char *cp;
    unsigned short ymax = 0;
    while(fgets(buf, 1024, f)) {
        int buflen = strlen(buf);
        cp = buf;
        for (int i = 0; i < buflen; i++) {
            if (buf[i] == ',') {
                buf[i] = '\0';
                unsigned short y = atoi(&buf[i + 1]);
                ymax = y > ymax ? y : ymax;
            }
            if (buf[i] == '-') {
                i += 2;
                cp = &buf[i];
            }
        }
    }

    // We used to get xmin and xmax above too, but part 2 mooted that.
    // Now we need to set up memory for the maps of the cave.
    int xmin = 0;
    int xmax = 1000;
    unsigned short xinterval = xmax - xmin + 3;
    unsigned short yinterval = ymax + 3;
    
    char **p1map = malloc(sizeof(char *) * yinterval);
    char **p2map = malloc(sizeof(char *) * yinterval);
    for (int i = 0; i < yinterval; i++) {
        p1map[i] = malloc(xinterval * sizeof(char));
        p2map[i] = malloc(xinterval * sizeof(char));
        memset(p1map[i], '.', xinterval * sizeof(char));
        memset(p2map[i], '.', xinterval * sizeof(char));
        p1map[i][xinterval - 1] = '\0';
        p2map[i][xinterval - 1] = '\0';
    }
    memset(p2map[yinterval - 1], '#', xinterval * sizeof(char));

    // Now that mem's set up, lets mark the maps with the input.
    fseek(f, 0, 0);
    int x, y;
    while(fgets(buf, 1024, f)) {
        cp = buf, x = 0, y = 0;
        int buflen = strlen(buf);
        for (int i = 0; i < buflen; i++) {
            if (buf[i] == ',') {
                if (x) {
                    record(p1map, x - xmin, y, atoi(cp) - xmin, atoi(&buf[i + 1]));
                    record(p2map, x - xmin, y, atoi(cp) - xmin, atoi(&buf[i + 1]));
                }
                x = atoi(cp);
                y = atoi(&buf[i + 1]);
            }
            if (buf[i] == '-') {
                i += 3;
                cp = &buf[i];
            }
        }
    }

    // And then drop sand until we can't.
    int cnt = 0;
    while(drop_sand(p1map, xmin, ymax)) {
        cnt++;
    }
    printf("part 1: %d\n", cnt);

    cnt = 0;
    while(drop_sand(p2map, xmin, ymax + 3)) {
        cnt++;
    }
    printf("part 2: %d\n", cnt);
}