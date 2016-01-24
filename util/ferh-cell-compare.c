/*      __obsolete__
 *
 * NAME
 *      ferh-cell-compare - find the total energy difference between a single
 *                          and doubled unit cell
 * SYNOPSIS
 *      ferh-cell-compare single_out double_out
 * DESCRIPTION
 *      Takes two output files from scf-extract-results.sh, one from a single
 *      unit cell and one from a doubled unit cell, and outputs the difference
 *      between the total energies if both were single unit cells.
 *
 *      Since the writing of half-column, this is now pretty obsolete, though
 *      is left in case it turns useful at some other point.
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

int
main (int argc, const char **argv)
{
    int linenum = 0;
    char linebuf[2][LINEBUF];
    double lattice_param[2];
    double energy[2];

    /* Check we've got the requisite number of input arguments. */
    if (argc != 3) {
        fprintf (stderr, "Usage: ferh-cell-compare small-file big-file\n");
        fprintf (stderr, "Files are assumed to have the same numbers of lines, and matching parameters.\n");
        return 1;
    }

    /* Open the files for reading. */
    FILE *small = fopen (argv[1], "r");
    FILE *big   = fopen (argv[2], "r");

    /* Check that both opened successfully. */
    if (!small) {
        fprintf (stderr, "Error: couldn't open %s for reading.\n", argv[1]);
        return 1;
    }
    if (!big) {
        fprintf (stderr, "Error: couldn't open %s for reading.\n", argv[2]);
        return 1;
    }

    /* Loop through both files simultaneously.  We assume that both have the
     * same numbers of comments and generally the exact same structure. */
    while (fgets (linebuf[0], LINEBUF, small)
           && fgets (linebuf[1], LINEBUF, big)) {
        /* Keep track of line number for error outputting. */
        linenum++;
        
        /* We assume that they're both the same, so check for a comment line. */
        if (linebuf[0][0] == '#') continue;

        /* Read in the parameters we care about.  We can ignore the
         * magnetisation and the Fermi energy because they're not important for
         * the comparison. */
        if (sscanf (linebuf[0], "%lf, %*f, %*f, %lf\n", lattice_param, energy) != 2) {
            fprintf (stderr, "Couldn't read all values on line %d of %s\n", linenum, argv[1]);
            return 1;
        }
        if (sscanf (linebuf[1], "%lf, %*f, %*f, %lf\n", lattice_param + 1, energy + 1) != 2) {
            fprintf (stderr, "Couldn't read all values on line %d of %s\n", linenum, argv[1]);
            return 1;
        }

        /* Check that the lattice parameters are actually the same. */
        if (lattice_param[0] != lattice_param[1]) {
            fprintf (stderr, "Lattice parameters different on line %d\n", linenum);
            return 1;
        }

        /* Print out the lattice parameter and the difference between the total
         * energies. */
        printf ("%.6g, %.6g\n", lattice_param[0], 0.5 * energy[1] - energy[0]);
    }

    /* Tidy up. */
    fclose (small);
    fclose (big);

    return 0;
}
