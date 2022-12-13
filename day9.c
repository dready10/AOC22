#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Ok, so I did a dumb dumb thing and I thought, "I don't want to be passing
// around all these C-tuples that aren't native, it's just a hassle. So instead
// I'll just jam the x coord in the upper half of a long and the y coord in the 
// lower half. This was a Bad Decision--it made debugging about 500 times
// harder, in part because just printing the location of any knot with any non-
// zero X coord is going to be a meaningless number. Not fun experiment, what
// I learned is to never do this again.
const long LOWER_MASK = 0x00000000ffffffffL;
const long UPPER_MASK = 0xffffffff00000000L;
const long YNEGBIT    = 0x0000000080000000L;
const long XNEGBIT    = 0x8000000000000000L;

// Ironically, despite going through all that to try to put two ints into one
// var, I wanted a convenience struct to keep track of nodes I had visited.
typedef struct visited {
    uint visited_count;
    uint buflen;
    long* visited;
} visited;

// Bitwise math to get/set X and Y. My favorite words are "platform" 
// and "dependency."
int getX(long pos) {
    long um = UPPER_MASK;
    int x = (pos & UPPER_MASK) >> 32;
    if (x & YNEGBIT) {
        return (x & (YNEGBIT ^ (LOWER_MASK))) * -1;
    }
    return x;
}

int getY(long pos) {
    int y = (pos & LOWER_MASK);
    if (y & YNEGBIT) {
        return (y & (YNEGBIT ^ LOWER_MASK)) * -1;
    }
    return y;

}

long setX(long pos, int x) {
    long new_x = abs(x);
    return (pos & LOWER_MASK) | ((new_x << 32) | (x < 0 ? XNEGBIT : 0));
}

long setY(long pos, int y) {
    long new_y = abs(y);
    return (pos & UPPER_MASK) | new_y | (y < 0 ? YNEGBIT : 0);
}

// This function moves tail knots in sequence. The rule is: if the "head"
// is in the same row/column, just move in a column/row. Otherwise, move
// the tail diagonally toward the head. 
void slither(long *snake, int snake_len) {
    for (uint j = 1; j < snake_len; j++) {
        int head_X = getX(snake[j-1]);
        int head_Y = getY(snake[j-1]);
        int tail_X = getX(snake[j]);
        int tail_Y = getY(snake[j]);

        int x_diff = head_X - tail_X;
        int y_diff = head_Y - tail_Y;

        if (abs(x_diff) == 2) {
            snake[j] = setX(snake[j], getX(snake[j]) + x_diff / 2);
            if (abs(y_diff)) {
                snake[j] = setY(snake[j], getY(snake[j]) + (y_diff > 0 ? 1 : -1));
            }
        } else if (abs(y_diff) == 2) {
            snake[j] = setY(snake[j], getY(snake[j]) + y_diff / 2);
            if (abs(x_diff)) {
                snake[j] = setX(snake[j], getX(snake[j]) + (x_diff > 0 ? 1 : -1));
            }
        }
    }
}

// Does what it says on the tin. 
void move_head(long *snake, int snake_len, visited *v, uint steps, char dir) {
    for (uint i = 0; i < steps; i++) {
        long prevTail = snake[snake_len - 1];
        switch(dir) {
            case 'U':
                snake[0] = setY(snake[0], getY(snake[0]) + 1);
                break;
            case 'D':
                snake[0] = setY(snake[0], getY(snake[0]) - 1);
                break;
            case 'L':
                snake[0] = setX(snake[0], getX(snake[0]) - 1);
                break;
            case 'R':
                snake[0] = setX(snake[0], getX(snake[0]) + 1);
                break;
            default:
                printf("Yipes: %c\n", dir);
                exit(1);
        }
        slither(snake, snake_len);
        if (snake[snake_len - 1] != prevTail) {
            v->visited[v->visited_count++] += snake[snake_len - 1];
        }
    }
}

int main() {
    // Set uppa the stuffa
    FILE *f = fopen("input/day9.txt", "r");
    if (errno) {
        printf("Couldn't open input/day9.txt: %d\n", errno);
        return errno;
    }

    long *p1snake = malloc(sizeof(long) * 2);
    p1snake[0] = 0;
    p1snake[1] = 0;
    long *p2snake = malloc(sizeof(long) * 10);
    for (int i = 0; i < 10; i++) {
        p2snake[i] = 0;
    }

    visited v;
    v.visited_count = 1;
    v.buflen = 100000;
    v.visited = malloc(sizeof(long) * v.buflen);
    v.visited[0] = p1snake[1];

    visited v2;
    v2.visited_count = 1;
    v2.buflen = 100000;
    v2.visited = malloc(sizeof(long) * v2.buflen);
    v2.visited[0] = p2snake[9];

    // Read the input
    char buf[6];
    while(fgets(buf, 6, f)) {
        buf[strlen(buf) - 1] = '\0'; //cut off \n
        move_head(p1snake, 2, &v, atoi(&buf[2]), buf[0]);
        move_head(p2snake, 10, &v2, atoi(&buf[2]), buf[0]);
    }

    // And then I was lazy when I created my convience struct
    // and just jam on any coord a tail visits, regardless of
    // if it's already visited. So now I have to go through and
    // actually count the distinct visited nodes.
    long* visited = malloc(sizeof(long) * v.buflen);
    uint uvc = 0;
    int seen;
    for (int i = 0; i < v.visited_count; i++) {
        seen = 0;
        // check to see if we've already visited the
        // node before
        for (int j = 0; j < uvc; j++) {
            if (visited[j] == v.visited[i]) {
                seen = 1;
                break;
            }
        }    
        // and if we haven't, add it to the list we've
        // seen.
        if (!seen) {
            visited[uvc++] = v.visited[i];
        }    
    }


    printf("Part 1: %d\n", uvc);
    uvc = 0;

    // Same counting for snake 2.
    for (int i = 0; i < v2.visited_count; i++) {
        seen = 0;
        for (int j = 0; j < uvc; j++) {
            if (visited[j] == v2.visited[i]) {
                seen = 1;
                break;
            }
        }    
        if (!seen) {
            visited[uvc++] = v2.visited[i];
        }    
    }
    printf("Part 2: %d\n", uvc);
    return 0;
}