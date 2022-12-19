#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// I think there might be a better way of track-typing, but because I have a void **
// in my struct, these seemed easier, more straightforward, and maybe easier-to-read.
#define LIST 1
#define INT 2

// Helper struct. The void** points to all children of the list, which can all be 
// either an int* or a list*.
// Children_types helps me remember which type is in children[i] so I know how to
// appropriately cast / dereference / recurse.
typedef struct list {
    void **children;
    int *children_types;
    unsigned int num_children;
    unsigned int buflen;
} list;

// Helper function for making lists.
list *make_list() {
    list *l = malloc(sizeof(list));
    l->buflen = 10;
    l->children = malloc(sizeof(void *) * l->buflen);
    l->children_types = malloc(sizeof(int *) * l->buflen);
    l->num_children = 0;
}

// Helper function to add children to a list. Automatically
// resizes children/children_types if the buflen is too short.
void add_child(list *l, void *child, int type) {
    if (l->num_children == l->buflen) {
        l->buflen += 10;
        l->children = realloc(l->children, sizeof(void *) * l->buflen);
        l->children_types = realloc(l->children, sizeof(int *) * l->buflen);
    }

    l->children[l->num_children] = child;
    l->children_types[l->num_children++] = type;
}

// Helper function for memory management.
void free_list(list *l) {
    for(int i = 0; i < l->num_children; i++) {
        if (l->children_types[i] == INT) {
            free(l->children[i]);
        } else {
            free_list((list *) l->children[i]);
        }
    }
    free(l->children_types);
    free(l->children);
    free(l);
}

// Where the magic happens. Recursively parse a list. If we hit a , and the previous
// character is not a ], we know we have a digit to add to the list. If we hit a [,
// we know we've hit a sub-list that we need to parse and then add to the parent. 
// This function returns the number of characters of *input that it "ate" so we jump
// ahead that many characters after parsing the list.
// If we hit a ], that means we've finished parsing the (sub)list, and can finish
// adding whatever digits we need and return the number of input characters we "ate."
int parse_list(char *input, list *parent) {
    unsigned int buflen = strlen(input);
    char *digitstart = &input[1];
    for (int i = 1; i < buflen; i++) {
        if (input[i] == ',' && input[i-1] != ']') {
            input[i] = '\0';
            int *t = malloc(sizeof(int));
            *t = atoi(digitstart);
            digitstart = &input[i + 1];
            add_child(parent, t, INT);
            continue;
        }
        if (input[i] == '[') {
            list *l = make_list();
            i += parse_list(&input[i], l);
            add_child(parent, l, LIST);
            digitstart = &input[i+2];
            continue;
        }
        if (input[i] == ']') {
            if (input[i - 1] == '[' || input[i-1] == ']') {
                return i;
            } 
            int *t = malloc(sizeof(int));
            *t = atoi(digitstart);
            add_child(parent, t, INT);
            return i;
        }
    }
}

// Recurse throught a list and compare using the rules of the problem. If the types are mixed,
// promote the int and compare the resulting two lists. Otherwise, if they're numbers, compare
// values and return if the lists are ordered or not, depending. If the values of the ints are
// the same, keep checking. When the values of all ints in a list are the same, then the shorter
// list must be on the left. If the lists are the same length, return -1 to show that the
// sublist did not decide orderedness and the parent should keep checking.
int is_ordered(list *l1, list *l2) {
    unsigned int iter_max = l1->num_children > l2->num_children ? l2->num_children : l1->num_children;
    for (int i = 0; i < iter_max; i++) {
        if (l1->children_types[i] != l2->children_types[i]) {
            list *promoted_int = make_list();
            int res;
            if (l1->children_types[i] == INT) {
                add_child(promoted_int, l1->children[i], INT);
                res = is_ordered(promoted_int, l2->children[i]);
            } else {
                add_child(promoted_int, l2->children[i], INT);
                res = is_ordered(l1->children[i], promoted_int);
            }

            free(promoted_int);
            if (res > -1) return res;
        } else if (l1->children_types[i] == INT) {
            if (*((int *) l1->children[i]) < *((int *)l2->children[i])) return 1;
            if (*((int *) l1->children[i]) > *((int *)l2->children[i])) return 0;
        } else {
            int res = is_ordered(l1->children[i], l2->children[i]);
            if (res > -1) return res;
        }
    }
    if (l1->num_children < l2->num_children) return 1;
    if (l1->num_children > l2->num_children) return 0;
    return -1;
}

// Function to sort the lists in part 2. This is probably technically O(n^2) but
// for an input of like 400 lists or so, that's fine.
void sort_lists(list **lists, int num_children) {
    for (int i = 1; i < num_children; i++) {
        int r = is_ordered(lists[i - 1], lists[i]);
        if (!is_ordered(lists[i - 1], lists[i])) {
            list *t = lists[i - 1];
            lists[i - 1] = lists[i];
            lists[i] = t;
            i = (i - 2) < 0 ? 0 : i - 2;
        }
    }
}

int main() {
    FILE *f = fopen("input/day13.txt", "r");
    if (errno) {
        printf("Could not open input/day13.txt: %d\n", errno);
        return errno;
    }

    // Part 1
    char buf[1024];
    list *l1, *l2;
    int toggle = 0;
    int index = 1;
    int summed_index = 0;
    while(fgets(buf, 1024, f)) {
        if (toggle == 0) {
            l1 = make_list();
            parse_list(buf, l1);
        }
        if (toggle == 1) {
            l2 = make_list();
            parse_list(buf, l2);
        }
        if (toggle == 2) {
            toggle = -1;
            if (is_ordered(l1, l2)) summed_index += index;
            index++;
        }
        toggle++;
    }
    printf("Part 1: %d\n", summed_index);

    // Part 2
    fseek(f, 0, 0);
    list **lists = malloc(sizeof(list *) * 500);
    list *code1, *code2;
    int i = 0;
    while(fgets(buf, 1024, f)) {
        if (buf[0] == '\n') continue;
        lists[i] = make_list();
        parse_list(buf, lists[i++]);
    }

    // Add the two divider packets
    lists[i] = make_list();
    code1 = lists[i];
    parse_list("[[2]]", lists[i++]);
    lists[i] = make_list();
    code2 = lists[i];
    parse_list("[[6]]", lists[i++]);

    // Then get the results.
    sort_lists(lists, i);
    int p2 = 1;
    for(int j = 0; j < i; j++) {
        if (lists[j] == code1 || lists[j] == code2) p2 *= (j + 1);
    }
    printf("Part 2: %d\n", p2);

    free(lists);
}