#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define MAX_LINE_LENGTH 1024

int has_consecutive_As(char *sequence, int count) {
    int consecutive = 0;
    for (int i = 0; i < strlen(sequence); i++) {
        if (sequence[i] == 'A') {
            consecutive++;
            if (consecutive >= count) return 1;
        } else {
            consecutive = 0;
        }
    }
    return 0;
}

void process_files(const char *input_path_R1, const char *input_path_R2, const char *output_path_R1, const char *output_path_R2) {
    gzFile in_R1 = gzopen(input_path_R1, "rb");
    gzFile in_R2 = gzopen(input_path_R2, "rb");
    gzFile out_R1 = gzopen(output_path_R1, "wb");
    gzFile out_R2 = gzopen(output_path_R2, "wb");

    if (!in_R1 || !in_R2 || !out_R1 || !out_R2) {
        perror("File opening failed");
        return;
    }

    char id_R1[MAX_LINE_LENGTH], id_R2[MAX_LINE_LENGTH];
    char sequence_R1[MAX_LINE_LENGTH], sequence_R2[MAX_LINE_LENGTH];
    char plus_R1[MAX_LINE_LENGTH], plus_R2[MAX_LINE_LENGTH];
    char quality_R1[MAX_LINE_LENGTH], quality_R2[MAX_LINE_LENGTH];

    while (gzgets(in_R1, id_R1, MAX_LINE_LENGTH) && gzgets(in_R2, id_R2, MAX_LINE_LENGTH)) {
        gzgets(in_R1, sequence_R1, MAX_LINE_LENGTH);
        gzgets(in_R1, plus_R1, MAX_LINE_LENGTH);
        gzgets(in_R1, quality_R1, MAX_LINE_LENGTH);

        gzgets(in_R2, sequence_R2, MAX_LINE_LENGTH);
        gzgets(in_R2, plus_R2, MAX_LINE_LENGTH);
        gzgets(in_R2, quality_R2, MAX_LINE_LENGTH);

        if (!has_consecutive_As(sequence_R2, 30)) {
            gzprintf(out_R1, "%s%s%s%s", id_R1, sequence_R1, plus_R1, quality_R1);
            gzprintf(out_R2, "%s%s%s%s", id_R2, sequence_R2, plus_R2, quality_R2);
        }
    }

    gzclose(in_R1);
    gzclose(in_R2);
    gzclose(out_R1);
    gzclose(out_R2);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <input_R1> <input_R2> <output_R1> <output_R2>\n", argv[0]);
        return 1;
    }

    process_files(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}