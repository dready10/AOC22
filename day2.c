#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Using the below enum values, the formula for score calculation
// is [win/lose/tie] * 3 + ([self-rock/paper/scissors] + 1)
// It's just a matter of getting the lookups right.
typedef enum play {
    ROCK = 0,
    PAPER = 1,
    SCISSORS = 2,
    LOSE = 0,
    TIE = 1,
    WIN = 2,
} play;

// So here are the lookups. It's a matrix accessed with matrix[opponents_play][self_play].
// Because Rock is 0, Paper is 1, and Scissors is 2, the lookup for opponent playing scissors
// and self playing rock is [2][0], or WIN in the first matrix.
// If the self play is WIN, that's [2][2], which is ROCK.
const int p1_win_matrix[3][3] = {{TIE, WIN, LOSE}, {LOSE, TIE, WIN}, {WIN, LOSE, TIE}};
const int p2_win_matrix[3][3] = {{SCISSORS, ROCK, PAPER}, {ROCK, PAPER, SCISSORS}, {PAPER, SCISSORS, ROCK}};

// So then we calculate the score depending on which part: win-type*3 + play-type + 1.
int p1_get_score(play self, play opp) {
    return p1_win_matrix[opp][self] * 3 + (self + 1);
}

// Self is a win-type, so just multiply by three, and then lookup
// the play we made and add one.
int p2_get_score(play self, play opp) {
    return self * 3 + (p2_win_matrix[opp][self] + 1);
}

// A/B/C always represents Rock, Paper, or Scissors.
// X/Y/Z is represented to be either Rock/Paper/Scissors or Lose/Tie/Win.
// So we just return the integer mapped to those values to communicate that.
play lookup_play(char inp) {
    switch(inp) {
        case 'A':
            return ROCK;
        case 'B':
            return PAPER;
        case 'C':
            return SCISSORS;
        case 'X':
            return 0;
        case 'Y':
            return 1;
        case 'Z':
            return 2;
        default:
            printf("lookup_play: invalid input: %c", inp);
            exit(1);
    }
}

// And finally the main function.
int main() {
    FILE* f = fopen("./input/day2.txt", "r");
    if (errno) {
        printf("Couldn't open day2.txt: %d\n", errno);
        return errno;
    }

    char buf[100];
    char* self;
    char* opp;
    int p1_score = 0;
    int p2_score = 0;

    // Get each line of the file (up to 100 chars), pull out the
    // opponent's and self play, and get the scores.
    while (fgets(buf, 100, f)) {
        opp = strtok(buf, " ");
        self = strtok(NULL, "\n");
        p1_score += p1_get_score(lookup_play(self[0]), lookup_play(opp[0]));
        p2_score += p2_get_score(lookup_play(self[0]), lookup_play(opp[0]));
    }
    printf("Part 1 Score: %d\n", p1_score);
    printf("Part 2 Score: %d\n", p2_score);
    return 0;
}