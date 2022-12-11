#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef PART
    #define PART 1
#endif

// Helpful struct to keep errthing together.
typedef struct monkey {
    int id;
    unsigned long inspections;
    unsigned long *items;
    unsigned int num_items;
    char *operation;
    int divisorTest;
    int trueTestTarget;
    int falseTestTarget;
} monkey;

// Helpful function to make sure we initialize everything good.
monkey *new_monkey(int id) {
    monkey *m = malloc(sizeof(monkey));
    m->id = id;
    m->inspections = 0;
    m->items = calloc(1024, sizeof(unsigned long int));
}

/* This is specifically written for the day's inputs. No need
in this case to spend time writing a general-purpose string calculator. 
operation is always in the form of "Operation: new = old [*+] [old|(d)+]"
so if we just look at the 23rd character and it's an o, we know the
operation is old * old. Otherwise, the arithmetic op is in operation[21] 
and the second operand for the op starts in operation[23]. */
unsigned long operate(char *operation, unsigned long old) {
    char *new_op = &operation[6];

    if (operation[23] == 'o') {
        return old * old; // special case for new = old * old
    }

    int operand2 = atoi(&operation[23]);
    if (operation[21] == '+') return old + operand2;
    if (operation[21] == '*') return old * operand2;
    printf("Ruh roh: %s\n", operation);
    exit(1);
}

// Helper function that removes the item being throw in index i
// from monkey "from" and adds it to the end of the queue in
// monkey "to". Not terribly efficient beacuse monkey->items
// is just an array and not a true queue, but it's fast enough
// for this problem.
void throw_item(int index_from, monkey *from, monkey *to) {
    int old_item_val = from->items[index_from];
    for (int i = index_from; i < from->num_items - 1; i++) {
        from->items[i] = from->items[i+1];
    }
    from->num_items--;

    to->items[to->num_items++] = old_item_val;
}

// Let the monkey take its turn.
void monkey_around(monkey *m, monkey **ms, int divisor) {
    if (m->num_items == 0) {
        return;
    }

    while(m->num_items) { // monkey always empties its queue on its turn
        m->inspections++;
        m->items[0] = operate(m->operation, m->items[0]);

        if (divisor == 3) { // divisor == 3 is part 1
            m->items[0] = floor(m->items[0] / divisor);
        } else { // basically any other divisor is part 2
            m->items[0] = m->items[0] % divisor;
        }

        if (m->items[0] % m->divisorTest) {
            throw_item(0, m, ms[m->falseTestTarget]);
        } else {
            throw_item(0, m, ms[m->trueTestTarget]);
        }
    }
}

// Helper function to parse the initial worry values of items monkey m holds
void get_ints(monkey *m, char *buf) {
    buf = &buf[18];
    int buflen = strlen(buf);
    char *tok = buf;

    for(int i = 0; i < buflen; i++) {
        if(buf[i] == ',') {
            buf[i] = '\0';
            m->items[m->num_items++] = atoi(tok);
            tok = &buf[i+2];
        }
    }
    if (atoi(tok)) m->items[m->num_items++] = atoi(tok);
}

int main() {
    FILE *f = fopen("input/day11.txt", "r");
    if (errno) {
        printf("Could not open input/day11.txt: %d\n", errno);
        return errno;
    }

    char buf[1024];
    monkey **monkeys = malloc(sizeof(monkey*) * 1024);
    int number_of_monkeys = 0;

    // Parse all the input and setup the data structures.
    while(fgets(buf, 1024, f)) {
        if (buf[0] == 'M') {
            monkeys[number_of_monkeys++] = new_monkey(atoi(&buf[7]));
            continue;
        }

        monkey *curr = monkeys[number_of_monkeys - 1];
        if (buf[2] == 'S') {
            get_ints(curr, buf);
            continue;
        }

        if (buf[2] == 'O') {
            char *buf2 = malloc(sizeof(char) * 1024);
            strcpy(buf2, &buf[2]);
            buf2[strlen(buf2) - 1] = '\0';
            curr->operation = buf2;
            continue;
        }
        
        if (buf[2] == 'T') {
            sscanf(buf, "  Test: divisible by %d\n", &curr->divisorTest);
            continue;
        }

        if (buf[7] == 't') {
            sscanf(buf, "    If true: throw to monkey %d\n", &curr->trueTestTarget);
            continue;
        }

        if (buf[7] == 'f') {
            sscanf(buf, "    If false: throw to monkey %d\n", &curr->falseTestTarget);
            continue;
        }

        printf("???: %s\n", buf);
    }


    int lcm;
    int rounds;

    //I didn't want to reparse all the input, or to save it somewhere, so I
    //just split the operation of the monkey game by whether we're in part 1.
    //Either compile with -DPART=1 (or 2) or change the #define at the top.

    if (PART == 1) { 
        lcm = 3;
        rounds = 20;
    } else {
        // Here's the key to part 2. All of tests are if the worry value is
        // divisible by x. That means if we take worry value % [least common
        // multiple of all divisors], the tests will always be unaffected. 
        // All the divisor tests are prime, so getting the LCM is just a matter
        // of multiplying all divisor tests together. Line 79 implements the %.
        for (int i = 0; i < number_of_monkeys; i++) {
            lcm *= monkeys[i]->divisorTest;
        }
        rounds = 10000;
    }

    // Let 'er rip.
    for (int round = 0; round < rounds; round++) {
        for (int monkey = 0; monkey < number_of_monkeys; monkey++) {
            monkey_around(monkeys[monkey], monkeys, lcm);
        }
    }

    // And then get the answer.
    unsigned long highest = 0;
    unsigned long second_highest = 0;
    for (int i = 0; i < number_of_monkeys; i++) {
        if (monkeys[i]->inspections > highest) {
            second_highest = highest;
            highest = monkeys[i]->inspections;
            continue;
        }
        if (monkeys[i]->inspections > second_highest) {
            second_highest = monkeys[i]->inspections;
        }
    }
    printf("Part %d: %lu\n", PART, highest * second_highest);
}
