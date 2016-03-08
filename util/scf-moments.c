#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILBYTES   10700
#define BUFSIZE     2048
#define NSITES      4
#define SEARCHDIST  256
#define DATASETSIZE 64

int
main (int argc, char **argv)
{
    char buf[BUFSIZE];
    char dataset[DATASETSIZE];
    double mag;
    char *cur, *prev = buf;

    if (argc != 2) {
        fprintf (stderr, "Usage: scf-moments dataset.out\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen (argv[1], "rb");
    if (!in) {
        fprintf (stderr, "Error: couldn't open '%s' for reading.\n", argv[1]);
        return EXIT_FAILURE;
    }

    strncpy (dataset, argv[1], DATASETSIZE - 1);
    dataset[DATASETSIZE - 1] = '\0';

    /* Remove trailing .out */
    dataset[strlen(dataset) - 4] = '\0';

    /* Change to CSV. */
    cur = dataset;
    while (*cur) {
        if (*cur == '-')
            *cur = ',';
        cur++;
    }

    fputs (dataset, stdout);

    /* Read in the tail-ed part of the file into the buffer. */
    fseek (in, -TAILBYTES, SEEK_END);
    fread (buf, BUFSIZE - 1, sizeof (buf[0]), in);
    /* Make sure to null-terminate. */
    buf[BUFSIZE - 1] = '\0';

    for (int i = 0; i < NSITES; i++) {
        cur = strstr (prev, "sum");
        if (!cur) {
            fprintf (stderr, "Couldn't find enough sites in '%s'.\n", argv[1]);
            return EXIT_FAILURE;
        }
        if (sscanf (cur, "sum %*f %*f %*f %lf", &mag) != 1) {
            fprintf (stderr, "Error getting magnetisation for site %d in '%s'.\n", i+1, argv[1]);
            return EXIT_FAILURE;
        }
        printf (",%.15g", mag);
        prev += SEARCHDIST;
    }
    putchar ('\n');

    fclose (in);
    return EXIT_SUCCESS;
}
