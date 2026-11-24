#include "header.h"

int remove_comments(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Error opening input file for comment removal");
        return -1;
    }

    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("Error opening output file for comment removal");
        fclose(in);
        return -1;
    }

    int c, next;
    int in_string = 0;
    int in_char = 0;

    while ((c = fgetc(in)) != EOF) {
        // Handle string literals so // or /* inside strings are ignored
        if (in_string) {
            fputc(c, out);
            if (c == '\\') {
                next = fgetc(in);
                if (next != EOF) fputc(next, out);
            } else if (c == '"') {
                in_string = 0;
            }
            continue;
        }

        // Handle character literals
        if (in_char) {
            fputc(c, out);
            if (c == '\\') {
                next = fgetc(in);
                if (next != EOF) fputc(next, out);
            } else if (c == '\'') {
                in_char = 0;
            }
            continue;
        }

        if (c == '"') {
            in_string = 1;
            fputc(c, out);
        } else if (c == '\'') {
            in_char = 1;
            fputc(c, out);
        } else if (c == '/') {
            next = fgetc(in);
            if (next == '/') {
                // Single-line comment: skip until newline
                while ((c = fgetc(in)) != EOF && c != '\n');
                if (c == '\n') fputc('\n', out);
            } else if (next == '*') {
                // Multi-line comment: skip until */
                int prev = 0;
                while ((c = fgetc(in)) != EOF) {
                    if (c == '\n') {
                        fputc('\n', out); // Preserve line structure
                    }
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    prev = c;
                }
            } else {
                fputc(c, out);
                if (next != EOF) ungetc(next, in);
            }
        } else {
            fputc(c, out);
        }
    }

    fclose(in);
    fclose(out);
    return 0;
}
