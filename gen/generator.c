#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"

static void usage (void);
static double strtod_checked (const char *str);
static FILE *fopen_checked (const char *filename, const char *mode, const char *description);

char output_inp_filename[MAX_FILENAME];
char output_pot_filename[MAX_FILENAME];

int
main (int argc, char **argv)
{
    /* Return code of the program. */
    int ret = EXIT_SUCCESS;

    /* The mode of the generator, either START or ITERATING. */
    int mode;
    /* The argument index which points to the current lattice parameter to be
     * converted. */
    int param_arg;
    /* Total number of lattice parameters */
    int nparams;
    
    /* Array of the converted lattice parameters. */
    double *lattice_params;

    /* Various file streams for reading and writing. */
    FILE *output_inp_file, *output_pot_file, *input_pot_new_file = NULL;

    /* We've got to have at least one argument to do anything! */
    if (argc == 1)
        usage ();

    /* Check which mode we're operating in. We assume that we're in start mode,
     * unless we specifically find the option for iterating. */
    mode = (strcmp (argv[1], "-i")) ? START : ITERATING;

    if (mode == ITERATING) {
        /* We'll need a .pot_new file and at least one lattice parameter. */
        if (argc < 4)
            usage ();

        input_pot_new_file = fopen_checked (argv[2], "r", "previous potential file");
        param_arg = 3;
    } else {
        /* We may or may not have the "-s" explicitly, so we need to check. */
        if (!strcmp (argv[1], "-s")) {
            if (argc < 3)
                usage ();

            param_arg = 2;
        } else {
            param_arg = 1;
        }
    }

    /* Calculate the number of lattice parameters we've been given, and check
     * that it's the correct number. */
    nparams = argc - param_arg;
    if (!correct_number_of_parameters (nparams))
        usage ();

    /* Allocate space for these lattice parameters. */
    lattice_params = malloc (nparams * sizeof (*lattice_params));
    if (!lattice_params) {
        fprintf (stderr, "Couldn't allocate memory to store lattice parameters.\n");
        exit (EXIT_FAILURE);
    }

    /* Do the conversion for each parameter into a double. */
    for (int i = 0; param_arg < argc; param_arg++, i++)
        lattice_params[i] = strtod_checked (argv[param_arg]);

    /* Generate the filenames for the two output files. */
    make_inp_filename (lattice_params);
    make_pot_filename (lattice_params);

    /* Open the output files for writing. */
    output_inp_file = fopen_checked (output_inp_filename, "w", "input file");
    output_pot_file = fopen_checked (output_pot_filename, "w", "potential file");

    /* Write the output files and check for success. */
    if (write_inp_file (output_inp_file, lattice_params, mode)
        || write_pot_file (output_pot_file, lattice_params, mode)) {
        ret = EXIT_FAILURE;
    }

    /* Handle the cleanup operations. */
    fclose (output_inp_file);
    fclose (output_pot_file);
    if (mode == ITERATING)
        fclose (input_pot_new_file);
    free (lattice_params);

    return ret;
}

#define USAGE_TEXT \
    "Usage: generator [-s|-i pot_new_file] lattice_parameter ...\n"\
    "\n"\
    "OPTIONS\n"\
    "-s\n"\
    "    Generate a start potential file, like xband would.  If no option is passed, this is assumed.\n"\
    "-i pot_new_file\n"\
    "    Generate an \"iterating\" potential file, based on a given .pot_new file.  Use in regions\n"\
    "    where convergence may be a problem.\n"\
    "\n"\
    "LATTICE PARAMETERS\n"\
    "The number of lattice parameters is variable, and will vary depending on the specific generator\n"\
    "in use.\n\n"

/* Print out usage information and exit the program. */
static void
usage (void)
{
    fputs (USAGE_TEXT, stderr);
    specific_usage ();

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

/* Perform an fopen call, exiting out with a failure message and code if it
 * fails. */
static FILE *
fopen_checked (const char *filename, const char *mode, const char *description)
{
    FILE *fp = fopen (filename, mode);
    /* Get a description of the operation to be performed. */
    char *mode_description;
    
    mode_description = (!strcmp (description, "r")) ? " for reading"
                       : (!strcmp (description, "w")) ? " for writing" : "";

    /* Test it opened correctly. */
    if (!fp) {
        fprintf (stderr, "Couldn't open %s '%s'%s.\n",
                 description, filename, mode_description);
        exit (EXIT_FAILURE);
    }

    return fp;
}
