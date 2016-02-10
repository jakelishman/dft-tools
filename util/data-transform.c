#include <stdio.h>
#include <stdlib.h>

#define LINEBUF (4096)
#define METRES_PER_A0 (5.29177211e-11)
#define JOULES_PER_RYDBERG (2.1798741e-18)

int
main (int argc, char **argv)
{
    if (argc != 2) {
        fprintf (stderr, "Usage: data-transform infile\n");
        exit (EXIT_FAILURE);
    }

    FILE *in = fopen (argv[1], "r");
    if (!in) {
        fprintf (stderr, "Couldn't open input file '%s'\n", argv[1]);
        exit (EXIT_FAILURE);
    }

    char buf[LINEBUF];
    double params[5];

    while (fgets (buf, sizeof(buf), in)) {
        if (buf[0] == '#') {
            puts (buf);
            continue;
        }

        sscanf (buf, "%lf, %lf, %lf, %lf, %lf\n", params, params + 1, params + 2, params + 3, params + 4);
        printf ("%.10g, %.10g, %.10g, %.10g, %.10g\n",
                METRES_PER_A0 * params[0], METRES_PER_A0 * params[0] * params[1],
                JOULES_PER_RYDBERG * params[2], JOULES_PER_RYDBERG * params[3],
                JOULES_PER_RYDBERG * params[4]);
    }

    fclose (in);

    return EXIT_SUCCESS;
}
