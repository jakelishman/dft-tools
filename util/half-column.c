/* NAME
 *      half-column - halves the total energy column
 * SYNOPSIS
 *      half-column results_file
 * DESCRIPTION
 *      A very quick, dirty and non-portable method of halving the final column
 *      (for total energy) in a file output by scf-extract-results.sh.  Mostly
 *      useful if you've got a doubled unit cell, and you want to compare it to
 *      the single unit cell.
 *
 * AUTHOR
 *      Jake Lishman
 * EMAIL
 *      j.lishman@warwick.ac.uk
 * DATE
 *      2016-01-18
 */

#include <stdio.h>

#define LINEBUF 2048

int main (int argc, char **argv)
{
    /* Check we've got the correct number of arguments. */
    if (argc != 2) {
        fprintf (stderr, "Usage: half-column results_file\n");
        return 1;
    }

    /* Open the given file for reading. */
    FILE *in = fopen (argv[1], "r");
    if (!in) {
        fprintf (stderr, "Couldn't open %s.\n", argv[1]);
        return 1;
    }

    /* We should do this better - what if the maximum length is greater than
     * LINEBUF - but this is quick and I couldn't be bothered. */
    char buf[LINEBUF];
    /* Storage for the different values.  Again, we should do this dynamically,
     * but really, who has the time? */
    double a, m, ef, e;

    /* Loop through the file. */
    while (fgets (buf, LINEBUF, in)) {
        /* Ignore lines that are comments. */
        if (buf[0] == '#') {
            puts (buf);
            continue;
        }

        /* Read the variables into our temporary storage. */
        sscanf (buf, "%lf, %lf, %lf, %lf\n", &a, &m, &ef, &e);

        /* Output them again in the correct form. */
        printf ("%g, %.15g, %.15g, %.15g\n", a, m, ef, 0.5 * e);
    }

    /* Done. */
    fclose (in);
    return 0;
}
