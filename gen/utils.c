#include "generator.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define LINE_LENGTH 8192

/* Seek forwards to a line in a file, returning an error if it couldn't be
 * found, or 0 on success. The file stream will be pointing to the line after
 * the given line after this function has been called. */
int
seek_to_line (FILE *file, const char *str)
{
    char line[LINE_LENGTH];

    /* Loop through lines (technically fgets might not get the whole line and we
     * could check this, but I haven't yet bothered). */
    while (fgets (line, LINE_LENGTH, file)) {
        /* Remove the '\n' from the buffer. */
        line [strlen (line) - 1] = '\0';

        /* Check if the line is an exact match. */
        if (!strcmp (line, str))
            return 0;
    }

    return 1;
}

/* Copy "count" lines from "out" to "in".  If count is negative, then all
 * remaining lines in "in" will be copied. */
int
copy_lines (FILE *out, FILE *in, long count)
{
    long left = 0;
    char line[LINE_LENGTH];
    /* Check if we wanted all the remaining lines. */
    const int inf = (count < 0) ? 1 : 0;
    /* Set count to an absurdly high value if we want all the lines.
     * Technically this could be broken, but I'm not worried about that now. */
    count = inf ? LONG_MAX : count;

    /* Do the copy operation. */
    while (count-- && fgets (line, LINE_LENGTH, in))
        fwrite (line, 1, strlen (line), out);

    /* Check how many lines were left. */
    if (!inf && count >= 0)
        left = count + 1;
    else
        left = 0;

    return left;
}

/* Advance the file stream "in" by "count" lines.  Returns 0 on success, of the
 * number of lines left to skip when we reached the end of the file. */
int
skip_lines (FILE *in, long count)
{
    static char tmp[LINE_LENGTH];

    /* Can't skip a negative number of lines! */
    if (count < 0) {
        fprintf (stderr, "Can't skip a negative number of lines!\n");
        exit (EXIT_FAILURE);
    }

    /* We just read in the lines, but we don't actually have to worry about
     * doing anything with them. */
    while (count-- && fgets (tmp, LINE_LENGTH, in));

    /* If we got to the end, then count == -1, so to return the number of lines
     * left when we ran out of file, we have to return count + 1. */
    return count + 1;
}
