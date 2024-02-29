#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define MAX_LINE_LENGTH 1024

void process_fastq(const char *input_file_r1, const char *input_file_r2, const char *output_file_r1, const char *output_file_r2) {
    char line_r1[4][MAX_LINE_LENGTH];
    char line_r2[4][MAX_LINE_LENGTH];
    int r1_modified;
    
    gzFile fin_r1 = gzopen(input_file_r1, "r");
    gzFile fout_r1 = gzopen(output_file_r1, "w");
    gzFile fin_r2 = gzopen(input_file_r2, "r");
    gzFile fout_r2 = gzopen(output_file_r2, "w");

    while (!gzeof(fin_r1) && !gzeof(fin_r2)) {
        r1_modified = 0;
        
        // Read 4 lines for R1 and R2
        for (int i = 0; i < 4; i++) {
            gzgets(fin_r1, line_r1[i], MAX_LINE_LENGTH);
            gzgets(fin_r2, line_r2[i], MAX_LINE_LENGTH);
            // Remove trailing newline characters
            line_r1[i][strcspn(line_r1[i], "\n")] = 0;
            line_r2[i][strcspn(line_r2[i], "\n")] = 0;
        }

        // Check and modify R1
        if (strstr(line_r1[1], "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTT")) {
            char new_id[MAX_LINE_LENGTH];
            snprintf(new_id, MAX_LINE_LENGTH, "%s+%.*s", line_r1[0], 8, line_r1[1]);
            gzprintf(fout_r1, "%s\n%s\n%s\n%s\n", new_id, line_r1[1] + 8, line_r1[2], line_r1[3] + 8);
            r1_modified = 1;
        } else {
            for (int i = 0; i < 4; i++) {
                gzprintf(fout_r1, "%s\n", line_r1[i]);
            }
        }

        // Check and modify R2
        char *a_seq = strstr(line_r2[1], "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        if (a_seq) {
            int index = a_seq - line_r2[1];
            char new_id[MAX_LINE_LENGTH];
            if (r1_modified) {
                snprintf(new_id, MAX_LINE_LENGTH, "%s+%.*s", line_r2[0], 8, line_r1[1]);
            } else {
                strcpy(new_id, line_r2[0]);
            }
            gzprintf(fout_r2, "%s\n%.*s\n%s\n%.*s\n", new_id, index, line_r2[1], line_r2[2], index, line_r2[3]);
        } else {
            char new_id[MAX_LINE_LENGTH];
            if (r1_modified) {
                snprintf(new_id, MAX_LINE_LENGTH, "%s+%.*s", line_r2[0], 8, line_r1[1]);
                gzprintf(fout_r2, "%s\n%s\n%s\n%s\n", new_id, line_r2[1], line_r2[2], line_r2[3]);
            } else {
                for (int i = 0; i < 4; i++) {
                    gzprintf(fout_r2, "%s\n", line_r2[i]);
                }
            }
        }
    }

    gzclose(fin_r1);
    gzclose(fout_r1);
    gzclose(fin_r2);
    gzclose(fout_r2);
}


int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Usage: %s <input_R1> <input_R2> <output_R1> <output_R2>\n", argv[0]);
        return 1;
    }

    process_fastq(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}
