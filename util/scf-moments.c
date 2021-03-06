#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILBYTES   12000
#define BUFSIZE     4096
#define NSITES      4
#define SEARCHDIST  128
#define DATASETSIZE 64

int
main (int argc, char **argv)
{
    char buf[BUFSIZE];
    char dataset[DATASETSIZE];
    double s,d,p,tot;
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
        cur = strstr (prev, "B_core\n");
        if (!cur) {
            fprintf (stderr, "Couldn't find enough sites in '%s'.\n", argv[1]);
            return EXIT_FAILURE;
        }
        sscanf (cur, "B_core\n"
                     " s  %*f %*f %*f %lf %*f %*f %*f s %*f\n"
                     " p  %*f %*f %*f %lf %*f %*f %*f ns %*f\n"
                     " d  %*f %*f %*f %lf %*f %*f %*f cor %*f\n"
                     "sum %*f %*f %*f %lf %*f %*f %*f v+c %*f\n",
                     &s, &p, &d, &tot);
        printf (",%.15g,%.15g,%.15g,%.15g", s, p, d, tot);
        prev = cur + SEARCHDIST;
    }
    putchar ('\n');

    fclose (in);
    return EXIT_SUCCESS;
}
