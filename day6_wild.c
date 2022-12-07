#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h> 


// lol. see comments below for what this is about.
void compile_code() {
    system("gcc -g -c -o ./lib/wild.o ./lib/wild.c && gcc -shared -o ./lib/wild.so ./lib/wild.o");
}

// So I decided to try a wild solution and dynamically
// write code based on the challenge and then compile it
// and execute that dynamically generated code to get to
// the solution. In short: not a serious answer, just
// having a laugh and trying something I've never tried
// before.
void generate_code(int n) {
    FILE *f = fopen("./lib/wild.c", "w");
    char *s = malloc(1000); //because I'm just playing around I'm
    char *t = malloc(10000); //not focusing on memory management. 
    char *u = malloc(1000); //Just give me lots, we have plenty.
    sprintf(s, "int permuted_ands_n() { return %d; }", n);
    sprintf(t, "int search_string(char* s) { if(");

    //this for-loop generates all the and clauses of the if statement
    //to check that each pair of characters from 0 to n (fxn argument)
    //are unequal. ie: s[0] != s[1] && s[0] != s[2] && s[1] != s[2]
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            sprintf(u, "s[%d] != s[%d] && ", i, j);
            strcat(t, u);
        }
    }
    t[strlen(t) - 4] = '\0';
    strcat(t, ") return 1; return 0; }"); // if we found no pairs matching, return true; else false
    fwrite(s, strlen(s), sizeof(char), f);
    fwrite(t, strlen(t), sizeof(char), f);
    free(s);
    free(t);
    free(u);
    fclose(f);
    compile_code();
}

int main() {
    FILE *f = fopen("input/day6.txt", "r");
    if (errno) {
        printf("Couldn't open file input/day6.txt: %d\n", errno);
        return errno;
    }

    int (*permuted_ands_n)();
    int (*search_string)(char*);

    // A lot of this is unnecessary, half-baked boilerplate to try to make it
    // easier to run for a first or second or nth time. But it's not intended
    // to be bullet-proof or safe in anyway. What it does, though, is it
    // opens the dynamically generated library (see above) and pulls out the
    // compiled functions for use below.
    void* lib = dlopen("./lib/wild.so", RTLD_LAZY);
    if (lib == NULL) {
        generate_code(4);
        lib = dlopen("./lib/wild.so", RTLD_LAZY);
    }
    permuted_ands_n = dlsym(lib, "permuted_ands_n");
    search_string = dlsym(lib, "search_string");
    if (permuted_ands_n == NULL || search_string == NULL || permuted_ands_n() != 4) {
        dlclose(lib);
        generate_code(4);
        lib = dlopen("./lib/wild.so", RTLD_LAZY);
        permuted_ands_n = dlsym(lib, "permuted_ands_n");
    }

    char buf[10240];
    int marker, message, len;
    fgets(buf, 10239, f);
    len = strlen(buf);

    // now that we have the functions, we go through the string, a character at a time
    // to find an n-length (4 in part 1, 14 in part 2) sequence of non-matching characters.
    // if that occurs at the 0th character. permuted_ands_n just returns whatever n- in
    // n-length is. we need to add that value on to i because, in the problem, if the
    // very first four characters are non-matching (i = 0), then the answer is "4", not 0.
    for (int i = 0; i < len - permuted_ands_n(); i++) {
        marker = search_string(&buf[i]);
        if (marker) {
            marker = i + permuted_ands_n();
            break;
        }
    }
    dlclose(lib);
    printf("Marker at: %d\n", marker); //print part 1 solution

    //then go through, generate the code, recompile it, reopen it, repull out the functions
    //and rerun the above algo.
    generate_code(14);
    lib = dlopen("./lib/wild.so", RTLD_LAZY);
    permuted_ands_n = dlsym(lib, "permuted_ands_n");
    search_string = dlsym(lib, "search_string");
    for (int i = 0; i < len - permuted_ands_n(); i++) {
        message = search_string(&buf[i]);
        if (message) {
            message = i + permuted_ands_n();
            break;
        }
    }
    dlclose(lib);
    printf("Message at: %d\n", message); //part 2
}