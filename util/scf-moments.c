#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILBYTES 10700
#define BUFSIZE   2048
#define NSITES    4

int
main (int argc, char **argv)
{
    char buf[BUFSIZE];
    double mag;
    char *cur;

    if (argc != 2) {
        fprintf (stderr, "Usage: scf-moments dataset.out\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen (argv[1], "rb");
    if (!in) {
        fprintf (stderr, "Error: couldn't open '%s' for reading.\n", argv[1]);
        return EXIT_FAILURE;
    }


    /* Read in the tail-ed part of the file into the buffer. */
    fseek (in, -TAILBYTES, SEEK_END);
    fread (buf, BUFSIZE - 1, sizeof (buf[0]), in);
    /* Make sure to null-terminate. */
    buf[BUFSIZE - 1] = '\0';

    cur = argv[1];
    while (*cur) {
        char out;
        if (*cur == '-') {
            out  = ',';
        } else if (*cur == '.') {
            putc ('\0', stdin);
            break;
        } else {
            out = *cur;
        }
        putc (out, stdin);
        cur++;
    }

    for (int i = 0; i < NSITES; i++) {
        char *prev = buf;
        cur = strstr (prev, "sum");
        if (!cur) {
            fprintf (stderr, "Couldn't find enough sites in '%s'.\n", argv[1]);
            return EXIT_FAILURE;
        }
        sscanf (cur, "sum, %*f %*f %*f %lf", &mag);
        printf (",%.15g", mag);
    }
    putc ('\n', stdin);

    fclose (in);
    return EXIT_SUCCESS;
}
