#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define FS_NAME_MAXLEN 1024
#define FS_MAX_CHILDREN 64

// This is the data structure I use. It's a tree that I can
// climb up with parent or down with children.
typedef struct fs_entry {
    char *name;
    int size;
    bool is_dir;
    struct fs_entry *parent;
    struct fs_entry **children;
    int num_children;
    int max_children;
} fs_entry;

fs_entry *root; // duh
fs_entry *cur_dir; // program state cd command requires

// This is just a utility function that makes sure we can have
// an unlimited number of children in any given dir.
void resize_parent(fs_entry *parent) {
    if (parent == NULL) return;
    if (parent->num_children == parent->max_children) {
        parent->max_children += FS_MAX_CHILDREN;
        parent->children = realloc(parent->children, 
                                    sizeof(void*) * parent->max_children);
    }
}

// Utility function to produce an fs_entry with some common default values.
// Called by mkdir and touch which set other fs_entry defaults.
fs_entry *make_fs_entry(fs_entry *parent, char *name) {
    fs_entry *child = malloc(sizeof(fs_entry));
    child->name = malloc(sizeof(char) * strlen(name));
    strcpy(child->name, name);
    child->parent = parent;
    child->num_children = 0;
    child->max_children = FS_MAX_CHILDREN;
    if (parent != NULL) {
        parent->children[parent->num_children++] = child;
    }
}

// Adds a directory named child to parent.
fs_entry *mkdir(fs_entry *parent, char *child) {
    resize_parent(parent);
    fs_entry *dir = make_fs_entry(parent, child);
    dir->size = 0;
    dir->is_dir = true;
    dir->children = malloc(sizeof(void*) * FS_MAX_CHILDREN);
    return dir;
}

// Adds a file named child to parent. Updates parent and all
// grandparent directory sizes to account for the new file.
fs_entry *touch(fs_entry *parent, char *child, int size) {
    resize_parent(parent);
    fs_entry *file = make_fs_entry(parent, child);
    file->size = size;
    while(parent != NULL) {
        parent->size += size;
        parent = parent->parent;
    }
    file->is_dir = false;
    file->children = 0;
    return file;
}

// Searches the current directory for a child named "ch". If
// it finds it, it returns a pointer to that fs_entry. Otherwise,
// return null.
fs_entry *get_child(char* ch) {
    for (int i = 0; i < cur_dir->num_children; i++) {
        if (strcmp(ch, cur_dir->children[i]->name) == 0) {
            return cur_dir->children[i];
        }
    }
    return NULL;
}

// Update the state of the current directory. I may have pre-emptively
// gone overkill on this, and tried to support fully-qualified filepaths
// because I didn't fully examine the input and thought, "well, surely
// the input will have something like cd /usr/bin/lib or cd ../../a".
// That's not actually part of the input it now seems, so those fully-
// qualified paths may or may not work (I haven't tested), but this
// works for "cd dir" and "cd .." and "cd /", so that's good enough for
// the problem.
void cd(char *dest) {
    
    //If this is a fully qualified domain, start by setting cur_dur to root.
    if (dest[0] == '/') {
        cur_dir = root;
        dest = &dest[1];
    }

    int len = 0; //length of the dir name between //
    char *cpy = malloc(sizeof(char) * FS_NAME_MAXLEN);
    fs_entry *e;

    while(dest[0]) { // while not at the end of the string
        if (dest[len] == '\0' || dest[len] == '/') { // look for the end or a /

            // we found the end or /. So let's do something with that dirname.
            strncpy(cpy, dest, len * sizeof(char));
            cpy[len] = '\0';

            // if the dirname is .., move up a path.
            if (strcmp(cpy, "..") == 0) {
                cur_dir = cur_dir->parent;
                if (cur_dir == NULL) cur_dir = root;
                dest = &dest[len];
                len = 0;
                continue;
            }

            // otherwise look for the dirname in the current directory.
            e = get_child(cpy);
            if (e == 0) {
                printf("%s does not exist in %s. exiting.\n", cpy, cur_dir->name);
                exit(1);
            } else if (e->is_dir) {
                cur_dir = e; //if found, move into the directory.
            } else {
                printf("Could not cd into %s: is not directory. Exiting.\n", e->name);
                exit(1);
            }
            dest = &dest[len]; // and then update where we're looking at in the fully qualified string
            len = 0;
        }
        len++;
    }
    free(cpy);
}

// Recurse the file tree for part 1. If a directory is
// less than 100k bytes, add it to *size.
void p1_recurse_tree(fs_entry *p, int* size) {
    if (p->is_dir) {
        p->size <= 100000 ? *size += p->size : 0;

        for (int i = 0; i < p->num_children; i++) {
            p1_recurse_tree(p->children[i], size);
        }
    }
}

// Recurse the file tree for part 2. If a directory is
// larger than the size needed, but smaller than the smallest
// directory that is large enough to delete seen to far, update that.
void p2_recurse_tree(fs_entry *p, int *smallest_size, int *needed) {
    if (p->is_dir) {
        if (p->size < *smallest_size && p->size >= *needed) {
            *smallest_size = p->size;
        }

        for (int i = 0; i < p->num_children; i++) {
            p2_recurse_tree(p->children[i], smallest_size, needed);
        }
    }
}

int main() {
    FILE *f = fopen("input/day7.txt", "r");
    char buf[1024];

    if (errno) {
        printf("Could not open input/day7.txt: %d\n", errno);
        return errno;
    }

    // initialize state
    root = mkdir(NULL, "/");
    cur_dir = root;

    // read the input line by line.
    while(fgets(buf, 1023, f)) {
        buf[strlen(buf) - 1] = '\0'; //cut off trailing \n

        if (buf[0] == '$') { // commands start with $
            if (buf[2] == 'c') { // $ cd has c as the secondchar.
                cd(&buf[5]);
            } else if (buf[2] == 'l') { // we don't actually need to do anything with ls.
                continue;
            } else {
                printf("unknown command: %s\n", buf);
            }
        } else if (buf[0] == 'd') {
            mkdir(cur_dir, &buf[4]); // if buf doesn't start with $ but with d, we're lsing a dir we need to make.
        } else {
            // and if none of those, we're lsing a file we need to touch. that's of the format size name, so
            // parse buffer accordingly.
            int space_location = 0;
            int len = strlen(buf);
            for (; space_location <= len; space_location++) {
                if (buf[space_location] == ' ') break;
            }
            if (space_location == len) {
                printf("malformed touch attempt: %s\n", buf);
                continue;
            }
            buf[space_location] = '\0';
            touch(cur_dir, &buf[space_location + 1], atoi(buf));
        }
    }

    // Great, we've parsed the input and set up the tree, so now just run the solutions
    // and print the output.
    int size = 0;
    p1_recurse_tree(root, &size);
    printf("Part 1: %d\n", size);

    int space_needed = 30000000 - (70000000 - root->size);
    int smallest_so_far = root->size;
    p2_recurse_tree(root, &smallest_so_far, &space_needed);
    printf("Part 2: %d\n", smallest_so_far);

    // I could spend the time here freeing all the malloc space... but
    // we're exiting anyway.
}