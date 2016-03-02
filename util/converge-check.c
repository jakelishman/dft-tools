/* Program to find the words 'cycle converged' from the end of a file (currently the final 255 bytes),
 * for use with the SPRKKR program - Chris Davis 17:39 24/01/2016
 *
 * Updated to simplify magic numbers: Jake Lishman 2016-03-02 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILBYTES 255

int
main (int argc, char *argv[])
{
    if (argc != 2) {
        fprintf (stderr, "Usage: converge-check dataset.out\n");
        return EXIT_FAILURE;
    }

    FILE *in = fopen (argv[1], "r");
    if (!in) {
        fprintf (stderr, "Couldn't open file '%s' for reading.\n", argv[1]);
        return EXIT_FAILURE;
    }

    char str[TAILBYTES + 1];

    /* Seek to TAILBYTES before the end of the file, then read in those bytes
     * into the array. */
    fseek(in, -TAILBYTES, SEEK_END);
    fread(str, sizeof (str[0]), TAILBYTES, in);

    /* Null-terminate the buffer to prevent overflow problems. */
    str[TAILBYTES] = '\0';

    fclose(in);

    /* Return 0 on success, non-zero on failure. */
    return !strstr (str, "cycle converged");
}
