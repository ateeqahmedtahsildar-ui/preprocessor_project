#include "header.h"



static FILE *open_header_file(const char *filename) {
    // Try current directory first
    FILE *fp = fopen(filename, "r");
    if (fp) return fp;

    // Fallback to standard system include path
    char sys_path[512];
    snprintf(sys_path, sizeof(sys_path), "/usr/include/%s", filename);
    return fopen(sys_path, "r");
}

int process_includes(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Error opening input file for file inclusion");
        return -1;
    }

    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("Error opening output file for file inclusion");
        fclose(in);
        return -1;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in)) {
        char *ptr = line;
        while (isspace((unsigned char)*ptr)) ptr++;

        if (strncmp(ptr, "#include", 8) == 0 && (isspace((unsigned char)ptr[8]) || ptr[8] == '<' || ptr[8] == '"')) {
            char *start = ptr + 8;
            while (isspace((unsigned char)*start)) start++;

            char end_char = 0;
            if (*start == '<') end_char = '>';
            else if (*start == '"') end_char = '"';

            if (end_char) {
                start++;
                char *end = strchr(start, end_char);
                if (end) {
                    char filename[256];
                    size_t len = (size_t)(end - start);
                    if (len < sizeof(filename)) {
                        strncpy(filename, start, len);
                        filename[len] = '\0';

                        FILE *inc = open_header_file(filename);
                        if (inc) {
                            int ch;
                            while ((ch = fgetc(inc)) != EOF) {
                                fputc(ch, out);
                            }
                            fputc('\n', out);
                            fclose(inc);
                            continue;
                        } else {
                            fprintf(stderr, "Warning: Could not open included file '%s'\n", filename);
                        }
                    }
                }
            }
        }
        fputs(line, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}
