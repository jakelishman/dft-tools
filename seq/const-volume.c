#include <stdio.h>
#include <stdlib.h>

#define SQUARE(a) ((a) * (a))
#define CUBE(a) ((a) * (a) * (a))

static void usage (void);
static double strtod_checked (const char *str);

int
main (int argc, char **argv)
{
    if (argc != 5)
        usage ();

    const double orig_a      = strtod_checked (argv[1]);
    const double start_a     = strtod_checked (argv[2]);
    const double inc         = strtod_checked (argv[3]);
    const double end_a       = strtod_checked (argv[4]);

    const double orig_vol = CUBE(orig_a);

    const int max = 1 + (0.5 * inc + end_a - start_a) / inc;

    for (int i = 0; i < max; i++) {
        double cur_a = (double) i * inc + start_a;
        double cur_c = orig_vol / SQUARE (cur_a);
        printf ("%.10g %.10g\n", cur_a, cur_c / cur_a);
    }

    return 0;
}

#define USAGE_STRING \
    "Usage: const-volume orig_a start_a inc end_a\n\n"\
    "PARAMETERS\n"\
    "  orig_a\n"\
    "    The base lattice parameter of the CsCl FeRh, to calculate the\n"\
    "    original volume.\n"\
    "  start_a\n"\
    "    The start value of the lattice parameter to use.  Will probably be\n"\
    "    somewhat close to orig_a.\n"\
    "  inc\n"\
    "    The increment to apply to the start value of a at each step.\n"\
    "  end_a\n"\
    "    The largest value of a to use.\n"

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
