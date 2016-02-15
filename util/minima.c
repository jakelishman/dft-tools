/*  NAME
 *      minima - find the keys associated with the minimum values in a
 *               comma-separate values file
 *  SYNOPSIS
 *      minima file.csv [key_column(=0) [value_column(=1)]]
 *
 *  DESCRIPTION
 *      Using the given columns for the key and value, finds the minimum value
 *      (in the value column), and returns a list of all keys which had that
 *      value.
 *
 *      The file must have comma-separated columns.  Lines beginning with a hash
 *      ('#') sign are treated as comments, and ignored.  Columns are labelled
 *      left-to-right in 0-indexed form (i.e the first column is column 0).
 *
 *      If values are not given for the key column or the value column, they
 *      default to checking columns numbered 0 and 1 respectively.
 *
 *  AUTHOR
 *      Jake Lishman
 *  EMAIL
 *      j.lishman@warwick.ac.uk
 *  DATE
 *      2015-11-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NMAXSTATIC 64
#define LINEBUF 2048

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

int checked_strtod (double *result, char *nptr);

int
main (int argc, const char **argv)
{
    void *tmp;
    double  static_storage[NMAXSTATIC];
    double minimum_value = HUGE_VAL;
    double *minima = static_storage;
    int nminima = 0, line_num = 0;
    int storage_size = NMAXSTATIC;
    char line_buffer[LINEBUF], *cur_ptr;
    long key_col = 0, value_col = 1, cur_col = 0;
    double key, value;
    bool got_key = false, got_value = false;
    FILE *in;

    /* Catch how many arguments we've been given. */
    switch (argc) {
    case 4:
        /* Convert the value in the 3rd argument to an integer */
        value_col = strtol (argv[3], (char **)&tmp, 10);
        if (tmp == argv[3]) {
            fprintf (stderr, "Couldn't convert the value column \"%s\" to an integer.\n", argv[3]);
            exit (EXIT_FAILURE);
        }
        /* Fall-through */
    case 3:
        /* Convert the 2nd argument into an integer. */
        key_col = strtol (argv[2], (char **)&tmp, 10);
        if (tmp == argv[2]) {
            fprintf (stderr, "Couldn't convert the key column \"%s\" to an integer.\n", argv[2]);
            exit (EXIT_FAILURE);
        }
        /* Fall-through */
    case 2:
        /* Attempt to open the givne file. */
        in = fopen (argv[1], "r");
        if (!in) {
            fprintf (stderr, "Couldn't open the file \"%s\" for reading.\n", argv[1]);
            exit (EXIT_FAILURE);
        }
        break;
    default:
        /* If more than 4 arguments, or fewer than 2, then the program hasn't
         * been called correctly */
        fprintf (stderr, "Usage: minima file.csv [key column [value column]]\n");
        exit (EXIT_FAILURE);
    }

    /* Loop through the file - set the current (column) pointer to the line, and
     * read the next line of the file into memory. */
    while ((cur_ptr = fgets (line_buffer, LINEBUF, in))) {
        line_num++;

        /* If the line is a comment or blank.*/
        if (line_buffer[0] == '#' || line_buffer[0] == '\n')
            continue;

        /* Reinitialise the Booleans so we can check we got them in this line. */
        got_key = got_value = false;

        /* Loop through the columns (don't bother with anything past the last
         * column we're interested in). */
        for (cur_col = 0; cur_col <= MAX (key_col, value_col); cur_col++) {
            /* If we're in the same column as the key. */
            if (cur_col == key_col) {
                /* If the conversion fails, checked_strtod returns "false", so
                 * this works either way */
                got_key = checked_strtod (&key, cur_ptr);
            }

            /* This is an "if", not an "else if", because we allow them to be
             * the same (useless as it may be). */
            if (cur_col == value_col) {
                got_value = checked_strtod (&value, cur_ptr);
            }


            /* Update cur_ptr to point to the next comma. */
            cur_ptr = strchr (cur_ptr, ',');
            /* If there is no more commas, then go to the next line, regardless
             * of whether we've found a key and value or not - we'll check
             * outside the loop. */
            if (!cur_ptr)
                break;
            /* If we're still here, make cur_ptr point to the space after the
             * comma, rather than the comma itself (otherwise the conversions
             * will fail). */
            cur_ptr++;
        }

        /* If we didn't find either a key or a value in this line, then we just
         * have to fail. */
        if (!(got_key && got_value)) {
            fprintf (stderr, "Couldn't get both key and value on line %d\n", line_num);
            exit (EXIT_FAILURE);
        }

        /* Rather messy, sorry!
         *
         * Update the array of minima with the new information */
        if (value < minimum_value) {
            /* If the value is lower than the current minimum, then we can store
             * it alone.  We don't need to zero the rest of the array, because
             * we're only going to print out the array up to nminima anyway. */
            nminima = 1;
            minima[0] = key;
            minimum_value = value;
        } else if (value == minimum_value) {
            /* If they've got the same minimum value, then we have to be smarter
             * about it, since we could run out of storage, and we don't want to
             * have to dynamically reallocate every single time. */
            nminima++;

            /* If we've got too many minima values to store, then we need to
             * reallocate the storage. */
            if (nminima > storage_size) {
                /* If we're still using the static storage, we can't pass the
                 * initial pointer to realloc, because it'll not copy the values
                 * over. */
                if (storage_size == NMAXSTATIC) {
                    minima = malloc (sizeof(double) * storage_size * 2);

                    if (!minima) {
                        fprintf (stderr, "Out of memory.\n");
                        exit (EXIT_FAILURE);
                    }

                    /* The stack and the heap are guaranteed to be in different
                     * places, so memcpy is safe in this situation. */
                    memcpy (minima, static_storage, sizeof(double) * NMAXSTATIC);
                } else {
                    /* If we're already using dynamically allocated memory, we
                     * can just pass the pointer to realloc and have it
                     * automatically handle the memcpy/memmove operation. */
                    minima = realloc (minima, sizeof(double) * storage_size * 2);

                    if (!minima) {
                        fprintf (stderr, "Out of memory.\n");
                        exit (EXIT_FAILURE);
                    }
                }

                /* Update the storage size to the new value */
                storage_size *= 2;
            }

            /* Now that we're sure we've got enough storage, we can happily
             * store our value away. */
            minima[nminima - 1] = key;
        }
    }

    /* Unlock the input file now that we're done with it. */
    fclose (in);

    /* Print out results */
    printf ("Minimum value: %.15g\n", minimum_value);
    printf ("Keys: %d\n", nminima);
    for (int i = 0; i < nminima; i++)
        printf ("* %.15g\n", minima[i]);

    return EXIT_SUCCESS;
}

/* Perform a strtod conversion, and return an int indicating whether or not it
 * succeeded.  Stores the result in the pointer to "result". */
int
checked_strtod (double *result, char *nptr)
{
    void *tmp = result;
    *result = strtod (nptr, (char **)&tmp);
    /* Check the success of strtod. */
    if (tmp == nptr)
        return false;
    return true;
}
