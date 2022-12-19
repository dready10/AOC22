#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Can represent a beacon or a scanner.
typedef struct coord {
    int x;
    int y;
} coord;

// In the get-line function, I come up with all the ranges on line y
// that are covered by a beacon. This function merges those ranges
// together so that I get the fewest number of ranges that represent
// the coverage of the beacons on whatever line was passed into get-line.
int merge_ranges(int *ranges, int *num_ranges) {
    int fin_r = 0;
    int *r = malloc(sizeof(int) * *num_ranges * 2);

    // To merge, we need a copy of our ranges that will store the merged
    // versions. That's in r, and fin_r counts those ranges. We cycle
    // through all of the ranges passed in and compare it to merged
    // ranges in r. If there's overlap, we just extend the range in r.
    // If there's no overlap, add that range to r.
    for (int i = 0; i < *num_ranges; i++) {
        int c = 0, ub = ranges[i * 2 + 1], lb = ranges[i * 2];
        for (int j = 0; j < fin_r; j++) {
            int oub = r[j * 2 + 1], olb = r[j * 2];
            // New range ub overlaps with a merged range
            if (ub >= olb && ub <= oub) {
                if (lb < olb) r[j*2] = lb;
                c = 1;
            }
            if (lb >= olb && lb <= oub) {
                if (ub > oub) r[j*2 + 1] = ub;
                c = 1;
            }

            // Merged range overlaps with the new range
            if (oub >= lb && oub <= ub) {
                r[j * 2 + 1] = ub;
                c = 1;
            }
            if (olb >= lb && olb <= ub) {
                r[j*2] = lb;
                c = 1;
            }
            if (c) break;
        }
        if (c) {
            continue;
        }
        // If we reached here, there was no overlap, so add the range.
        // (c tracks if there was an overlap)
        r[fin_r * 2] = ranges[i * 2];
        r[fin_r++ * 2 + 1] = ranges[i * 2 + 1];
    }

    // Ok, we've looped through everything. But those merged ranges may
    // yet need more merging! So merge again if anything was merged.
    if (fin_r != *num_ranges) {
        merge_ranges(r, &fin_r);
    }

    // Ok, we have merged everything, and now we just need to return the
    // merged list in r, and give the number of ranges.
    *num_ranges = fin_r;
    memcpy(ranges, r, sizeof(int) * fin_r * 2);
    free(r);
}

// So the covered area of any given line is measured by the distance
// between the scanner and the beacon minus the distance between the
// scanner and the line we are looking at. So, for a given line n,
// go through the scanners/beacons, and get the coverage for line n.
// xv is for part 2 and is populated with the x coordinate if there
// is more than one range in a line.
int get_line(coord *beacons, coord *scanners, int line, int num_beacons, int *xv) {
    int *ranges = malloc(sizeof(int) * num_beacons * 2);
    int num_ranges = 0;
    for (int i = 0; i < num_beacons; i++) {
        int range = abs(scanners[i].y - beacons[i].y) +
                    abs(scanners[i].x - beacons[i].x);

        range -= abs(scanners[i].y - line);
        if (range < 0) {
            ranges[i * 2] = 0;
            ranges[i * 2 + 1] = 0;
        } else {
            ranges[num_ranges * 2] = scanners[i].x - range;
            ranges[num_ranges * 2 + 1] = scanners[i].x + range;
            num_ranges++;
        }
    }

    // Now we have all the ranges of coverage, we need to merge them.
    merge_ranges(ranges, &num_ranges);

    // And then the coverage on any given line is going to be the upper
    // bound - lower bound + 1 of all the ranges.
    int cnt = 0;
    for (int i = 0; i < num_ranges; i++) {
        cnt += ranges[i * 2 + 1] - ranges[i * 2] + 1;
    }

    // However, a twist. Scanners and beacons for some reason don't count
    // toward the total? So I have to take them out, which means looking
    // at each beacon and scanner, seeing if it's in the range, and if it
    // is in the range, taking it out. But also beacons repeat in the input
    // so I have to keep track of beacons I've seen and NOT cnt-- for
    // any beacon I have already seen. This... could use refactoring, but it works.
    coord *seen = malloc(sizeof(coord) * num_beacons);
    int num_seen = 0;

    for (int i = 0; i < num_beacons; i++) {
        for (int j = 0; j < num_ranges; j++) {
            if (beacons[i].y == line &&
                beacons[i].x >= ranges[j * 2] && beacons[i].x <= ranges[j * 2 + 1]) {
                    int s = 0;
                    for (int k = 0; k < num_seen; k++) {
                        if (beacons[i].y == seen[k].y && beacons[i].x == seen[k].x) {
                            s = 1;
                        }
                    };
                    if (!s) {
                        cnt--;
                        seen[num_seen].y = beacons[i].y;
                        seen[num_seen++].x = beacons[i].x;
                    }
                }
            if (scanners[i].y == line &&
                scanners[i].x >= ranges[j * 2] && scanners[i].x <= ranges[j * 2 + 1])
                cnt--;
        }
    }

    // For part 2, if we have more than one range, that means
    // we are on a line that is the answer. So return the relevant x-coord.
    if (num_ranges > 1) {
        if (ranges[0] < ranges[2]) *xv = ranges[2] - 1;
        else *xv = ranges[0] - 1;
    }
    return cnt;
}

int main() {
    FILE *f = fopen("input/day15.txt", "r");
    if (errno) {
        printf("Couldn't open input/day15.txt: %d\n", errno);
        return errno;
    }

    // Read everything in.
    char buf[1024];
    coord *beacons = malloc(sizeof(coord) * 50);
    coord *scanners = malloc(sizeof(coord) * 50);

    int bx, by, sx, sy;
    int inputs = 0;
    while(fgets(buf, 1024, f)) {
        sscanf(buf, "Sensor at x=%d, y=%d: closest beacon is at x=%d, y=%d\n", &sx, &sy, &bx, &by);
        beacons[inputs].x = bx;
        beacons[inputs].y = by;
        scanners[inputs].x = sx;
        scanners[inputs++].y = sy;               
    }

    // Then get the lines.
    int x_coord = 0;
    printf("Part 1: %d\n", get_line(beacons, scanners, 2000000, inputs, &x_coord));

    x_coord = 0;
    for (int j = 0; j < 4000000; j++) {
        get_line(beacons, scanners, j, inputs, &x_coord);
        if (x_coord != 0) {
            printf("Part 2: %lu\n", 4000000 * (unsigned long) x_coord + j);
            break;
        }
    }
}