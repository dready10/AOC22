#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper struct to keep track of nodes we have visited
typedef struct coord {
    int x;
    int y;
} coord;

// Helper struct to store the maze and the visited map tracking visits
typedef struct map {
    int num_rows;
    int num_cols;
    short **map;
} map;

// Take the top of the stack and look around at each adjacend node. If you can
// step there and it hasn't been visited, record that it takes X + 1 steps to 
// get there (where X is the steps it took to get to the current node).
// If it HAS been visited, but X + 1 is less than the number of steps it took
// to get there when it was previously visited, update the number of steps
// it takes to get there to X + 1 and then step there (so that all other
// nodes visited via this path get their step numbers updated).
void dfs(coord *visited_stack, int stack_height, map *maze, map *visited) {
    coord curr = visited_stack[stack_height - 1];
    coord *next = malloc(sizeof(coord));
    next->y = curr.y;
    next->x = curr.x;

    int next_step_height = stack_height + 1;

    if (curr.x > 0 && maze->map[curr.y][curr.x - 1] - maze->map[curr.y][curr.x] <= 1) {
        if (visited->map[curr.y][curr.x - 1] == 0 || visited->map[curr.y][curr.x - 1] > next_step_height) {
            visited->map[curr.y][curr.x - 1] = next_step_height;
            next->x--;
            visited_stack[stack_height++] = *next;
            dfs(visited_stack, stack_height, maze, visited);
            stack_height--;
            next->x++;
        }
    }

    if (curr.x < maze->num_cols - 1 && maze->map[curr.y][curr.x + 1] - maze->map[curr.y][curr.x] <= 1) {
        if (visited->map[curr.y][curr.x + 1] == 0 || visited->map[curr.y][curr.x + 1] > next_step_height) {
            visited->map[curr.y][curr.x + 1] = next_step_height;
            next->x++;
            visited_stack[stack_height++] = *next;
            dfs(visited_stack, stack_height, maze, visited);
            stack_height--;
            next->x--;
        }
    }

    if (curr.y > 0 && maze->map[curr.y - 1][curr.x] - maze->map[curr.y][curr.x] <= 1) {
        if (visited->map[curr.y - 1][curr.x] == 0 || visited->map[curr.y - 1][curr.x] > next_step_height) {
            visited->map[curr.y - 1][curr.x] = next_step_height;
            next->y--;
            visited_stack[stack_height++] = *next;
            dfs(visited_stack, stack_height, maze, visited);
            stack_height--;
            next->y++;
        }
    }

    if (curr.y < maze->num_rows - 1 && maze->map[curr.y + 1][curr.x] - maze->map[curr.y][curr.x] <= 1) {
        if (visited->map[curr.y + 1][curr.x] == 0 || visited->map[curr.y + 1][curr.x] > next_step_height) {
            visited->map[curr.y + 1][curr.x] = next_step_height;
            next->y++;
            visited_stack[stack_height++] = *next;
            dfs(visited_stack, stack_height, maze, visited);
            stack_height--;
            next->y--;
        }
    }

    free(next);
}

int main() {
    FILE *f = fopen("input/day12.txt", "r");
    if (errno) {
        printf("Couldn't open input/day12.txt: %d\n", errno);
        return errno;
    }

    // Set everything up.
    char buf[1024];
    int rows = 0;
    map maze, visited;
    coord start, end, *visited_stack;
    maze.map = malloc(sizeof(short*) * 100);
    visited.map = malloc(sizeof(short*) * 100);

    while(fgets(buf, 1024, f)) {
        int buflen = strlen(buf);
        buf[buflen-1] = '\0';

        short *row = malloc(sizeof(short) * buflen);
        short *rowv = calloc(buflen, sizeof(short));

        for (int i = 0; i < buflen; i++) {
            if (buf[i] == 'S') {
                start.x = i;
                start.y = rows;
                buf[i] = 'a' - 1;
            }
            if (buf[i] == 'E') {
                end.x = i;
                end.y = rows;
                buf[i] = 'z';
            }
            row[i] = buf[i];
        }

        maze.map[rows] = row;
        visited.map[rows++] = rowv;        
    }

    maze.num_cols = strlen(buf);
    maze.num_rows = rows;
    visited.num_cols = maze.num_cols;
    visited.num_rows = maze.num_rows;

    // And then run part 1.
    visited_stack = malloc(sizeof(coord*) * strlen(buf) * rows);
    visited_stack[0] = start;
    visited.map[start.y][start.x] = 1;
    dfs(visited_stack, 1, &maze, &visited);
    printf("Part 1: %d steps\n", visited.map[end.y][end.x] - 1);


    // Part 2. I just iterate on y and don't look at every single x/y combo
    // because the map only has "b"s in column 2. Thus, any other a on the
    // map outside of columns 1 or 3 can't reach the end point. I don't need
    // to check column 3 because any answer that would start there would
    // have an equivalent length value in column 1. Column 1 is all "a"s,
    // so no need for a check to make sure I'm starting at a weird place.
    int shortest_hike = visited.map[end.y][end.x] - 1;
    for (int y = 0; y < maze.num_rows; y++) {
        for(int i = 0; i < visited.num_rows; i++) {
            for (int j = 0; j < visited.num_cols; j++) {
                visited.map[i][j] = 0;
            }
        }
        start.x = 0;
        start.y = y;
        visited_stack[0] = start;
        visited.map[start.y][start.x] = 1;
        dfs(visited_stack, 1, &maze, &visited);
        if (shortest_hike > visited.map[end.y][end.x] && visited.map[end.y][end.x] != 0) {
            shortest_hike = visited.map[end.y][end.x];
        } 
    }
    printf("Part 2: %d steps\n", shortest_hike - 1);
}