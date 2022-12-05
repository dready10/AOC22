#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Convenience structure to help simplify my mental model.
typedef struct column {
    char *containers;
    int length;
    int buflen;
} column;

// Gotta roll your own in C.
void strrev(char *str) {
    int len = strlen(str);
    for(int i = 0; i < len / 2; i++) {
        char t = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = t;
    }
}

// Convenience function. Sets up the initial memory state
// for columns.
void initialize_columns(column *columns, int number) {
    for(int i = 0; i < number; i++) {
        columns[i].containers = malloc(sizeof(char) * 100);
        columns[i].containers[0] = '\0';
        columns[i].buflen = 100;
        columns[i].length = 0;
    }
}

// Adds the container addition to the top of the column.
void add_to_column(column *column, char addition) {
    if(column->length + 1 >= column->buflen) {
        column->containers = realloc(column->containers,
                                     sizeof(char) * (column->buflen + 100));
        column->buflen += 100;
    }
    column->containers[column->length++] = addition;
    column->containers[column->length] = '\0';
}

// Pops a container off the top of the column.
char pop_from_column(column *column) {
    if (column->length <= 0) {
        printf("pop_from_column: trying to pop from empty column.\n");
        return '\0';
    }
    char ret = column->containers[column->length-1];
    column->containers[column->length-1] = '\0';
    column->length--;
    return ret;
}

// Moving for part1--just pop off the top of the from and add it to the to.
void p1_move_containers(column *from, column *to, int number) {
    for (int i = 0; i < number; i++) {
        add_to_column(to, pop_from_column(from));
    }
}

// Moving for part 2--pop all of them off into one queue
// reverse the queue, and then add them all back on.
void p2_move_containers(column *from, column *to, int number) {
    char *moves = malloc(sizeof(char) * number + 1);
    moves[number] = '\0';
    for(int i = 0; i < number; i++) {
        moves[i] = pop_from_column(from);
    }
    strrev(moves);
    for(int i = 0; i < number; i++) {
        add_to_column(to, moves[i]);
    }
    free(moves);
}

// The container labels occur in the 1st and every 4th thereafter column of
// the input buffer, so this goes through and each of those and adds it in
// column-order onto the columns.
void populate_columns(column *columns, char *buf) {
    int buflen = strlen(buf);
    for (int i = 1; i < buflen; i += 4) {
        if (buf[i] != ' ') {
            add_to_column(&columns[(i-1)/4], buf[i]);
        }
    }
}

// Another helper function. Reads the first lines of file f until it
// hits a newline (which delineates the initial column state versus)
// move instructions. From the first line it calculates how many 
// columns there will be in total, allocates the columns, and then
// populates the columns with the container labels.
column *get_columns(FILE *f, int *num_columns) {
    column *columns;
    char buf[100];
    fgets(buf, 100, f);

    *num_columns = strlen(buf) / 4;
    columns = malloc(sizeof(column) * (*num_columns));
    initialize_columns(columns, *num_columns);
    populate_columns(columns, buf);

    while(fgets(buf, 100, f)) {
        if (buf[0] == '\n') {
            break;
        }
        populate_columns(columns, buf);
    }

    // We've added the containers in the reverse order (because of how
    // the file is read) as well as the numeric column id. Column ids
    // are just sequential, so pop them off and then reverse the containers.
    for (int i = 0; i < *num_columns; i++) {
        pop_from_column(&columns[i]);
        strrev(columns[i].containers);
    }
    return columns;
}

int main() {
    FILE *f = fopen("input/day5.txt", "r");
    if (errno) {
        printf("Couldn't open file input/day4.txt: %d\n", errno);
        return errno;
    }

    int num_columns = 0;
    column *p1_columns = get_columns(f, &num_columns);
    // A quick hack to set up the columns for part 2--just read from
    // the beginning of the file again and get_columns.
    fseek(f, 0, 0);
    column *p2_columns = get_columns(f, &num_columns);

    // get_columns leaves f after having read the newline. So the next
    // fgets will be the first move instruction. Parse the move instruction
    // and then perform it.
    char buf[100];
    int i = 0;
    while(fgets(buf, 100, f)) {
        int number, from, to;

        sscanf(buf, "move %d from %d to %d\n", &number, &from, &to);
        from--, to--;

        p1_move_containers(&p1_columns[from], &p1_columns[to], number);
        p2_move_containers(&p2_columns[from], &p2_columns[to], number);
    }

    // And print the outputs.
    printf("Part 1: ");
    for(int i = 0; i < num_columns; i++) {
        printf("%c", p1_columns[i].containers[p1_columns[i].length - 1]);
    }
    printf("\n");

    printf("Part 2: ");
    for(int i = 0; i < num_columns; i++) {
        printf("%c", p2_columns[i].containers[p2_columns[i].length - 1]);
    }
    printf("\n");

    for (int i = 0; i < num_columns; i++) {
        free(p1_columns[i].containers);
        free(p2_columns[i].containers);
    }
    free(p1_columns);
    free(p2_columns);
    return 0;
}