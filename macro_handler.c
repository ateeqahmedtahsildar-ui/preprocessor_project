
#include "header.h"

typedef struct {
    char name[128];
    int is_function;
    int param_count;
    char params[MAX_PARAMS][64];
    char body[1024];
} Macro;

static Macro macros[MAX_MACROS];
static int macro_count = 0;

static int is_ident_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static void trim_whitespace(char *str) {
    char *end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }
}

static void add_macro(const char *line) {
    if (macro_count >= MAX_MACROS) return;

    const char *p = line;
    while (isspace((unsigned char)*p)) p++;
    p += 7; // Skip "#define"
    while (isspace((unsigned char)*p)) p++;

    Macro *m = &macros[macro_count];
    memset(m, 0, sizeof(Macro));

    // Read macro name
    int i = 0;
    while (is_ident_char(*p) && i < 127) {
        m->name[i++] = *p++;
    }
    m->name[i] = '\0';
    if (i == 0) return;

    // Check for function-like macro: '(' immediately after name
    if (*p == '(') {
        m->is_function = 1;
        p++;
        while (*p && *p != ')') {
            while (isspace((unsigned char)*p)) p++;
            int pi = 0;
            while (is_ident_char(*p) && pi < 63) {
                m->params[m->param_count][pi++] = *p++;
            }
            m->params[m->param_count][pi] = '\0';
            if (pi > 0) m->param_count++;

            while (isspace((unsigned char)*p)) p++;
            if (*p == ',') p++;
        }
        if (*p == ')') p++;
    }

    // Read macro body
    while (isspace((unsigned char)*p)) p++;
    strncpy(m->body, p, sizeof(m->body) - 1);
    trim_whitespace(m->body);

    macro_count++;
}

static Macro *find_macro(const char *name) {
    for (int i = macro_count - 1; i >= 0; i--) {
        if (strcmp(macros[i].name, name) == 0) {
            return &macros[i];
        }
    }
    return NULL;
}

static void expand_function_macro(const Macro *m, char args[MAX_PARAMS][256], char *out, size_t out_size) {
    const char *p = m->body;
    size_t out_idx = 0;

    while (*p && out_idx < out_size - 1) {
        if (isalpha((unsigned char)*p) || *p == '_') {
            char token[128];
            int ti = 0;
            while (is_ident_char(*p) && ti < 127) {
                token[ti++] = *p++;
            }
            token[ti] = '\0';

            int replaced = 0;
            for (int i = 0; i < m->param_count; i++) {
                if (strcmp(token, m->params[i]) == 0) {
                    size_t arg_len = strlen(args[i]);
                    if (out_idx + arg_len < out_size - 1) {
                        strcpy(out + out_idx, args[i]);
                        out_idx += arg_len;
                    }
                    replaced = 1;
                    break;
                }
            }
            if (!replaced) {
                size_t tok_len = strlen(token);
                if (out_idx + tok_len < out_size - 1) {
                    strcpy(out + out_idx, token);
                    out_idx += tok_len;
                }
            }
        } else {
            out[out_idx++] = *p++;
        }
    }
    out[out_idx] = '\0';
}

static void expand_line(const char *line, FILE *out) {
    const char *p = line;
    int in_string = 0;

    while (*p) {
        if (*p == '"') {
            in_string = !in_string;
            fputc(*p++, out);
            continue;
        }
        if (in_string) {
            if (*p == '\\' && *(p + 1)) {
                fputc(*p++, out);
            }
            fputc(*p++, out);
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char token[128];
            int ti = 0;
            while (is_ident_char(*p) && ti < 127) {
                token[ti++] = *p++;
            }
            token[ti] = '\0';

            Macro *m = find_macro(token);
            if (m && !m->is_function) {
                fputs(m->body, out);
            } else if (m && m->is_function && *p == '(') {
                p++; // Skip '('
                char args[MAX_PARAMS][256];
                int arg_idx = 0;
                int depth = 1;
                int char_idx = 0;

                while (*p && depth > 0) {
                    if (*p == '(') {
                        depth++;
                        if (char_idx < 255) args[arg_idx][char_idx++] = *p;
                    } else if (*p == ')') {
                        depth--;
                        if (depth > 0 && char_idx < 255) {
                            args[arg_idx][char_idx++] = *p;
                        }
                    } else if (*p == ',' && depth == 1) {
                        args[arg_idx][char_idx] = '\0';
                        trim_whitespace(args[arg_idx]);
                        arg_idx++;
                        char_idx = 0;
                    } else {
                        if (char_idx < 255) args[arg_idx][char_idx++] = *p;
                    }
                    p++;
                }
                args[arg_idx][char_idx] = '\0';
                trim_whitespace(args[arg_idx]);

                char expanded[2048];
                expand_function_macro(m, args, expanded, sizeof(expanded));
                fputs(expanded, out);
            } else {
                fputs(token, out);
            }
        } else {
            fputc(*p++, out);
        }
    }
}

int process_macros(const char *input_file, const char *output_file) {
    FILE *in = fopen(input_file, "r");
    if (!in) {
        perror("Error opening input file for macro processing");
        return -1;
    }

    FILE *out = fopen(output_file, "w");
    if (!out) {
        perror("Error opening output file for macro processing");
        fclose(in);
        return -1;
    }

    macro_count = 0;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), in)) {
        char *ptr = line;
        while (isspace((unsigned char)*ptr)) ptr++;

        if (strncmp(ptr, "#define", 7) == 0 && isspace((unsigned char)ptr[7])) {
            add_macro(ptr);
            continue; 
        }
        expand_line(line, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}
