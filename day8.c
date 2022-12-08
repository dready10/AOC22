#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 99
#define NUM_ROWS 99

// There might be a more cleverer way to do this, but sometimes
// straightforward, simple, and slightly less efficient given
// a known problem domain is better than cleverer and complexer.
// Basically, from each direction, traverse along marking trees
// as visible or not if they are taller than all previous trees
// (or not).
void check_visibility(char **map, int **visibility_map) {
    int tallest;

    //check from east
    for (int i = 0; i < NUM_ROWS; i++) {
        tallest = -1;
        for (int j = 0; j < LINE_LENGTH; j++) {
            if (map[i][j] - '0' > tallest) {
                visibility_map[i][j] = 1;
                tallest = map[i][j] - '0';
            } 
        }
    }

    //check from west
    for (int i = 0; i < NUM_ROWS; i++) {
        tallest = -1;
        for (int j = LINE_LENGTH - 1; j >= 0; j--) {
            if (map[i][j] - '0' > tallest) {
                visibility_map[i][j] = 1;
                tallest = map[i][j] - '0';
            }
        }
    }

    //check from north
    for (int i = 0; i < LINE_LENGTH; i++) {
        tallest = -1;
        for (int j = 0; j < NUM_ROWS; j++) {
            if (map[j][i] - '0' > tallest) {
                visibility_map[j][i] = 1;
                tallest = map[j][i] - '0';
            }
        }
    }

    //check from south
    for (int i = 0; i < LINE_LENGTH; i++) {
        tallest = -1;
        for (int j = NUM_ROWS - 1; j >= 0; j--) {
            if (map[j][i] - '0' > tallest) {
                visibility_map[j][i] = 1;
                tallest = map[j][i] - '0';
            }
        }
    }
};

// Straightforward. From a given (x, y) position, look
// each direction and count the number of spaces "moved"
// until you hit a tree that is equal height to (x, y)'s.
int get_scenic_score(char **map, int y, int x) {
    int scores[4] = {0, 0, 0, 0};
    // look north
    for (int i = y - 1; i >= 0; i--) {
        scores[0]++;
        if (map[i][x] >= map[y][x]) break;
    }

    // look south
    for (int i = y + 1; i < NUM_ROWS; i++) {
        scores[1]++;
        if (map[i][x] >= map[y][x]) break;
    }

    // look east
    for (int i = x + 1; i < LINE_LENGTH; i++) {
        scores[2]++;
        if (map[y][i] >= map[y][x]) break;
    }

    // look west
    for (int i = x - 1; i >= 0; i--) {
        scores[3]++;
        if (map[y][i] >= map[y][x]) break;
    }

    return (scores[0] * scores[1] * scores[2] * scores[3]);
}

// Iterate through the entire map getting each scenic score. Keep track of the highest.
int get_highest_scenic_score(char** map) {
    int highest_score = -1;
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < LINE_LENGTH; j++) {
            int scenic_score = get_scenic_score(map, i, j);
            highest_score = highest_score < scenic_score ? scenic_score : highest_score;
        }
    }
}

// Helper function for part 1.
int count_visible(int** map) {
    int visible = 0;
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < LINE_LENGTH; j++) {
            if (map[i][j] != 0) {
                visible++;
            }
        }
    }
    return visible;
}

// Helper function to debug part 1. Just prints visible trees
// in blue and invisible trees in normal.
void print_map(char** map, int** visibility_map) {
    for (int i = 0; i < NUM_ROWS; i++) {
        for (int j = 0; j < LINE_LENGTH; j++) {
            if (visibility_map[i][j]) {
                printf("\033[44m%c\033[0m", map[i][j]);
            } else {
                printf("%c", map[i][j]);
            }
        }
        printf("\n");
    }
}

int main() {
    FILE* f = fopen("input/day8.txt", "r");
    if (errno) {
        printf("Could not open input/day8.txt: %d\n", errno);
        return errno;
    }

    // Set everything up. Read everything in.
    char **map = malloc(sizeof(char*) * NUM_ROWS);
    char **lineptr = map;
    char buf[LINE_LENGTH + 2];
    while(fgets(buf, LINE_LENGTH + 2, f)) {
        buf[LINE_LENGTH] = '\0';
        char *cpy = malloc(sizeof(char*) * LINE_LENGTH);
        strcpy(cpy, buf);
        *lineptr = cpy;
        lineptr++;
    }

    int **visibility_map = calloc(NUM_ROWS, sizeof(int*));
    for(int i = 0; i < NUM_ROWS; i++) {
        visibility_map[i] = calloc(LINE_LENGTH, sizeof(int));
    }

    // Then do the actual work.
    check_visibility(map, visibility_map);
    print_map(map, visibility_map);
    printf("Part 1: %d\n", count_visible(visibility_map));
    printf("Part 2: %d\n", get_highest_scenic_score(map));
}