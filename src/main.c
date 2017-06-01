/**
 * @author      
 * @file        main.c
 * @version     0.0.0-r0
 * @since       
 * 
 * @brief       A brief documentation about the file.
 *
 * A detailed documentation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

/**/
#define FPRINTF(file, fmt, cnt, ...)\
        for (int i = 0; i < cnt; i++) { fprintf(file, "%c", ' '); } fprintf(file, fmt, ##__VA_ARGS__)

/* V A R I A B L E S                                                          */


/* C O D E                                                                    */
void transpile(FILE *in, FILE *out) {
        int indent = 0;
        int val;
        char chr;

        FPRINTF(out, "#include <stdio.h>\n", indent);
        FPRINTF(out, "int main(void) {\n", indent);
        indent += 4;
        FPRINTF(out, "char tape[30000] = { 0 };\n", indent);
        FPRINTF(out, "char *pc = &tape[0];\n", indent);
        while ((val = fgetc(in)) != EOF) {
                chr = (char) (val & 0xff);
                switch(chr) {
                        case '>':
                                FPRINTF(out, "pc++;\n", indent);
                                break;
                        case '<':
                                FPRINTF(out, "pc--;\n", indent);
                                break;
                        case ',':
                                FPRINTF(out, "*pc = getc(stdin);\n", indent);
                                break;
                        case '.':
                                FPRINTF(out, "printf(\"%%c\", *pc);\n", indent);
                                break;
                        case '[':
                                FPRINTF(out, "while (*pc) {\n", indent);
                                indent += 4;
                                break;
                        case ']':
                                indent -= 4;
                                FPRINTF(out, "}\n", indent);
                                break;
                        case '+':
                                FPRINTF(out, "(*pc)++;\n", indent);
                                break;
                        case '-':
                                FPRINTF(out, "(*pc)--;\n", indent);
                                break;
                        default:
                                // FPRINTF(out, "/* failed to parse char: %c */", indent, chr);
                                break;
                }
        }

        FPRINTF(out, "return 0;\n", indent);
        indent -=4;
        FPRINTF(out, "}\n", indent);
        fflush(out);
}

int main(int argc, char **argv) {
        char opt;
        char flags = 0;
        char *in_s = "-";
        char *out_s = "-";
        char *command = NULL;
        FILE *in = stdin;
        FILE *out = stdout;
        FILE *process;

        while ((opt = getopt(argc, argv, "i:o:c")) != -1) {
                switch (opt) {
                        case 'i': /* input file specified */
                                in = fopen(optarg, "r");
                                if (in == NULL) {
                                        fprintf(stderr, "Failed to open file: %s\n", optarg);
                                        return 1;
                                }
                                in_s = optarg;
                                break;
                        case 'o': /* output file specified */
                                out = fopen(optarg, "w");
                                if (out == NULL) {
                                        fprintf(stderr, "Failed to open file: %s\n", optarg);
                                        return 1;
                                }
                                out_s = optarg;
                                break;
                        case 'c': /* compile? */
                                flags |= 1;
                                if (out !=  stdout) {
                                        fclose(out);
                                }
                                break;
                }
        }

        if (flags & 1) {
                command = malloc(strlen(out_s) + 20);
                strcpy(command, "gcc -o ");
                strcat(command, out_s);
                strcat(command, " -xc -");
                process = popen(command, "r+");
                out = process;
                free(command);
        }

        transpile(in, out);

        if (flags & 1)
                pclose(out);
        else
                fclose(out);

        fclose(in);
        return 0;
}
