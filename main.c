
#include"header.h"

static void build_output_filename(const char *input, char *output, size_t size) {
    strncpy(output, input, size - 3);
    output[size - 3] = '\0';

    char *dot = strrchr(output, '.');
    if (dot && strcmp(dot, ".c") == 0) {
        strcpy(dot, ".i");
    } else {
        strcat(output, ".i");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <inputfile.c>\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    char output_file[512];
    build_output_filename(input_file, output_file, sizeof(output_file));

    const char *temp1 = ".temp_no_comments.tmp";
    const char *temp2 = ".temp_included.tmp";

    if (remove_comments(input_file, temp1) != 0) {
        return 1;
    }

    if (process_includes(temp1, temp2) != 0) {
        remove(temp1);
        return 1;
    }

    if (process_macros(temp2, output_file) != 0) {
        remove(temp1);
        remove(temp2);
        return 1;
    }

    remove(temp1);
    remove(temp2);

    printf("Preprocessing complete. Output written to %s\n", output_file);
    return 0;
}
