#include <stdio.h>
#include <stdlib.h>

#define NX 50
#define MAXSTATES 10
#define LINEBUF 1024

int
main (int argc, char **argv)
{
    if (argc != 2) {
        fprintf (stderr, "Usage: agr-to-csv full-dos.agr\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen (argv[1], "r");
    if (!in) {
        fprintf (stderr, "Error: couldn't open '%s' for reading\n", argv[1]);
        return EXIT_FAILURE;
    }

    int atom = 1, i = 0;
    double data[NX][MAXSTATES + 1];
    char buf[LINEBUF];

    while (fgets (buf, LINEBUF, in)) {
        if (buf[0] == '#' || buf[0] == '@')
            continue;
        if (buf[0] == '&') {
            atom++;
            if (atom == MAXSTATES + 1)
                break;
            i = 0;
            continue;
        }

        if (i == NX) {
            fprintf (stderr, "Error: tried to read too many values of x\n");
            return EXIT_FAILURE;
        }

        if (atom == 1)
            sscanf (buf, " %lf %lf", &data[i][0], &data[i][atom]);
        else
            sscanf (buf, " %*f %lf", &data[i][atom]);

        i++;
    }

    fclose (in);

    for (i = 0; i < NX; i++)
        for (int j = 0; j < atom; j++)
            printf ("%.15g%c", data[i][j], (j == atom - 1) ? '\n' : ',');


    return EXIT_SUCCESS;
}
