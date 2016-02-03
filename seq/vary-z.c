#include <stdio.h>
#include <stdlib.h>

static void usage (void);
static double strtod_checked (const char *str);

int
main (int argc, char **argv)
{
    if (argc != 5)
        usage ();

    const double a       = strtod_checked (argv[1]);
    const double start_c = strtod_checked (argv[2]);
    const double inc     = strtod_checked (argv[3]);
    const double end_c   = strtod_checked (argv[4]);

    const int max = 1 + (0.5 * inc + end_c - start_c) / inc;

    for (int i = 0; i < max; i++)
        printf ("%.10g %.10g\n", a, ((double) i * inc + start_c) / a);

    return 0;
}

#define USAGE_STRING \
    "Usage: vary-z a start_c inc end_c\n\n"\
    "PARAMETERS\n"\
    "  a\n"\
    "    The base lattice parameter of the CsCl FeRh.\n"\
    "  start_c\n"\
    "    The start value of the vertical lattice parameter to use.  Will probably be\n"\
    "    somewhat close to a.\n"\
    "  inc\n"\
    "    The increment to apply to the start value of c at each step.\n"\
    "  end_c\n"\
    "    The largest value of c to use.\n"

static void
usage (void)
{
    fputs (USAGE_STRING, stderr);
    exit (EXIT_FAILURE);
}

/* Perform an strtod call, exiting out with a failure message and code if it
 * fails. */
static double
strtod_checked (const char *str)
{
    /* Set test to a known non-NULL pointer. */
    char *test = (char *) str;
    double out = strtod (str, &test);

    /* Test for success. */
    if (test == str) {
        fprintf (stderr, "Couldn't convert '%s' to a double.\n", str);
        exit (EXIT_FAILURE);
    }

    return out;
}
