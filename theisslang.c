/*
 * Theisslang Interpreter - Native C Implementation
 *
 * A Turing-complete programming language where all operations are variations of "Mr. Theiss".
 * Compiles to Brainfuck for execution.
 *
 * This is a native C implementation that can be compiled to a standalone executable
 * without any Python dependencies.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_TAPE_SIZE 30000
#define MAX_CODE_SIZE 1000000
#define MAX_TOKEN_SIZE 100
#define MAX_LINE_SIZE 10000

/* Token mapping structure */
typedef struct {
    const char *token;
    char bf_char;
} TokenMap;

/* Token mappings */
static const TokenMap TOKENS[] = {
    {"Mr. Theiss", '>'},
    {"Mr Theiss", '<'},
    {"MR. THEISS!", '+'},
    {"mr. theiss...", '-'},
    {"Mister Theiss?", '.'},
    {"Mister Theiss.", ','},
    {"Professor Theiss", '['},
    {"Student Theiss", ']'},
    {NULL, 0} /* Sentinel */
};

/* Function prototypes */
char *strip_paren_comments(const char *src);
char *expand_emphasis(const char *token);
char *theiss_to_bf(const char *src);
int *build_bracket_map(const char *code);
void run_bf(const char *code, const char *input);
void print_usage(const char *program_name);

/*
 * Remove parenthetical comments from source code
 */
char *strip_paren_comments(const char *src) {
    size_t len = strlen(src);
    char *out = malloc(len + 1);
    if (!out) return NULL;

    size_t out_idx = 0;
    int depth = 0;

    for (size_t i = 0; i < len; i++) {
        if (src[i] == '(') {
            depth++;
        } else if (src[i] == ')') {
            depth = depth > 0 ? depth - 1 : 0;
        } else if (depth == 0) {
            out[out_idx++] = src[i];
        }
    }

    out[out_idx] = '\0';
    return out;
}

/*
 * Handle run-length encoding for repeated operations
 * Returns newly allocated string or NULL if no expansion needed
 */
char *expand_emphasis(const char *token) {
    size_t len = strlen(token);

    /* Handle increment repetition: MR. THEISS! with multiple ! */
    if (strncmp(token, "MR. THEISS", 10) == 0) {
        int bangs = 0;
        for (size_t i = 10; i < len && token[i] == '!'; i++) {
            bangs++;
        }
        if (bangs > 0) {
            char *result = malloc(bangs + 1);
            if (!result) return NULL;
            memset(result, '+', bangs);
            result[bangs] = '\0';
            return result;
        }
    }

    /* Handle decrement repetition: mr. theiss... with multiple . */
    if (strncmp(token, "mr. theiss", 10) == 0) {
        int dots = 0;
        for (size_t i = 10; i < len && token[i] == '.'; i++) {
            dots++;
        }
        if (dots > 0) {
            char *result = malloc(dots + 1);
            if (!result) return NULL;
            memset(result, '-', dots);
            result[dots] = '\0';
            return result;
        }
    }

    return NULL; /* No expansion */
}

/*
 * Translate theisslang source to Brainfuck
 */
char *theiss_to_bf(const char *src) {
    char *stripped = strip_paren_comments(src);
    if (!stripped) return NULL;

    char *bf_code = malloc(MAX_CODE_SIZE);
    if (!bf_code) {
        free(stripped);
        return NULL;
    }

    size_t bf_idx = 0;
    char line[MAX_LINE_SIZE];
    char *line_ptr = stripped;

    /* Process line by line */
    while (*line_ptr && bf_idx < MAX_CODE_SIZE - 1) {
        /* Extract next line */
        size_t line_len = 0;
        while (*line_ptr && *line_ptr != '\n' && line_len < MAX_LINE_SIZE - 1) {
            line[line_len++] = *line_ptr++;
        }
        if (*line_ptr == '\n') line_ptr++;
        line[line_len] = '\0';

        /* Skip empty lines and handle # comments */
        char *comment_pos = strchr(line, '#');
        if (comment_pos) *comment_pos = '\0';

        /* Trim whitespace */
        char *start = line;
        while (*start && isspace(*start)) start++;
        char *end = start + strlen(start) - 1;
        while (end > start && isspace(*end)) *end-- = '\0';

        if (!*start) continue;

        /* Tokenize with greedy multi-word matching */
        char *parts[MAX_TOKEN_SIZE];
        int part_count = 0;

        /* Split into parts first */
        char *part = strtok(start, " \t");
        while (part && part_count < MAX_TOKEN_SIZE) {
            parts[part_count++] = part;
            part = strtok(NULL, " \t");
        }

        /* Greedily match tokens */
        int i = 0;
        while (i < part_count && bf_idx < MAX_CODE_SIZE - 1) {
            bool matched = false;

            /* Try emphasis expansion with progressively longer chunks */
            for (int j = 1; j <= 3 && i + j <= part_count; j++) {
                /* Reconstruct chunk */
                char chunk[MAX_TOKEN_SIZE * 10] = "";
                for (int k = 0; k < j; k++) {
                    if (k > 0) strcat(chunk, " ");
                    strcat(chunk, parts[i + k]);
                }

                /* Try emphasis expansion */
                char *expanded = expand_emphasis(chunk);
                if (expanded) {
                    strcpy(bf_code + bf_idx, expanded);
                    bf_idx += strlen(expanded);
                    free(expanded);
                    i += j;
                    matched = true;
                    break;
                }

                /* Try direct token match */
                for (const TokenMap *tm = TOKENS; tm->token; tm++) {
                    if (strcmp(chunk, tm->token) == 0) {
                        bf_code[bf_idx++] = tm->bf_char;
                        i += j;
                        matched = true;
                        break;
                    }
                }
                if (matched) break;
            }

            if (!matched) {
                /* Unknown token sequence - skip one word */
                i++;
            }
        }
    }

    bf_code[bf_idx] = '\0';
    free(stripped);
    return bf_code;
}

/*
 * Build bracket matching map for Brainfuck loops
 * Returns array where map[i] = matching position for bracket at i
 */
int *build_bracket_map(const char *code) {
    size_t len = strlen(code);
    int *map = malloc(len * sizeof(int));
    if (!map) return NULL;

    int *stack = malloc(len * sizeof(int));
    if (!stack) {
        free(map);
        return NULL;
    }

    size_t stack_size = 0;

    for (size_t i = 0; i < len; i++) {
        map[i] = -1;

        if (code[i] == '[') {
            stack[stack_size++] = i;
        } else if (code[i] == ']') {
            if (stack_size > 0) {
                size_t j = stack[--stack_size];
                map[i] = j;
                map[j] = i;
            } else {
                fprintf(stderr, "Syntax error: Unmatched ']' at position %zu\n", i);
                free(stack);
                free(map);
                exit(1);
            }
        }
    }

    if (stack_size > 0) {
        fprintf(stderr, "Syntax error: Unmatched '[' at position %d\n", stack[stack_size - 1]);
        free(stack);
        free(map);
        exit(1);
    }

    free(stack);
    return map;
}

/*
 * Execute Brainfuck code
 */
void run_bf(const char *code, const char *input) {
    size_t code_len = strlen(code);
    int *brackets = build_bracket_map(code);
    if (!brackets) {
        fprintf(stderr, "Failed to build bracket map\n");
        exit(1);
    }

    unsigned char *tape = calloc(MAX_TAPE_SIZE, sizeof(unsigned char));
    if (!tape) {
        free(brackets);
        fprintf(stderr, "Failed to allocate tape\n");
        exit(1);
    }

    size_t ptr = 0;
    size_t ip = 0;
    size_t input_idx = 0;

    while (ip < code_len) {
        char cmd = code[ip];

        switch (cmd) {
            case '>':
                ptr = (ptr + 1) % MAX_TAPE_SIZE;
                break;

            case '<':
                ptr = (ptr == 0) ? MAX_TAPE_SIZE - 1 : ptr - 1;
                break;

            case '+':
                tape[ptr]++;
                break;

            case '-':
                tape[ptr]--;
                break;

            case '.':
                putchar(tape[ptr]);
                break;

            case ',':
                if (input && input[input_idx]) {
                    tape[ptr] = input[input_idx++];
                } else {
                    tape[ptr] = 0; /* EOF */
                }
                break;

            case '[':
                if (tape[ptr] == 0) {
                    ip = brackets[ip];
                }
                break;

            case ']':
                if (tape[ptr] != 0) {
                    ip = brackets[ip];
                }
                break;

            default:
                /* Ignore unknown characters */
                break;
        }

        ip++;
    }

    free(tape);
    free(brackets);
}

/*
 * Print usage information
 */
void print_usage(const char *program_name) {
    fprintf(stderr, "Theisslang Interpreter v1.0\n");
    fprintf(stderr, "A Turing-complete language based on 'Mr. Theiss' variations\n\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s <program_file> [input_string]\n", program_name);
    fprintf(stderr, "  %s -e '<program_code>' [input_string]\n\n", program_name);
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s examples/hello_world.theiss\n", program_name);
    fprintf(stderr, "  %s -e 'MR. THEISS! Mister Theiss?'\n", program_name);
    fprintf(stderr, "  %s examples/cat.theiss 'Hello World'\n\n", program_name);
}

/*
 * Read entire file into string
 */
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(file_size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';

    fclose(file);
    return content;
}

/*
 * Main function
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    char *program_code = NULL;
    char *input_string = (argc >= 3) ? argv[2] : NULL;

    if (strcmp(argv[1], "-e") == 0) {
        /* Execute code directly from command line */
        if (argc < 3) {
            fprintf(stderr, "Error: -e requires program code\n");
            return 1;
        }
        program_code = argv[2];
        input_string = (argc >= 4) ? argv[3] : NULL;
    } else {
        /* Read from file */
        program_code = read_file(argv[1]);
        if (!program_code) {
            return 1;
        }
    }

    /* Translate to Brainfuck */
    char *bf_code = theiss_to_bf(program_code);
    if (!bf_code) {
        fprintf(stderr, "Failed to translate to Brainfuck\n");
        if (program_code != argv[2]) free(program_code);
        return 1;
    }

    /* Debug output */
    fprintf(stderr, "Compiled to Brainfuck: %s\n", bf_code);

    /* Execute */
    run_bf(bf_code, input_string);

    /* Cleanup */
    free(bf_code);
    if (program_code != argv[2]) free(program_code);

    return 0;
}
