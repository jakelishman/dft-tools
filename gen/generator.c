#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "generator.h"

static void usage (void);
static double strtod_checked (const char *str);
static FILE *fopen_checked (const char *filename, const char *mode, const char *description);

char output_inp_filename[MAX_FILENAME];
char output_pot_filename[MAX_FILENAME];
char dataset[MAX_FILENAME];

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
    double *params;

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
    params = malloc (nparams * sizeof (*params));
    if (!params) {
        fprintf (stderr, "Couldn't allocate memory to store lattice parameters.\n");
        exit (EXIT_FAILURE);
    }

    /* Do the conversion for each parameter into a double. */
    for (int i = 0; param_arg < argc; param_arg++, i++)
        params[i] = strtod_checked (argv[param_arg]);

    /* Get the name of the dataset. */
    make_dataset (params);

    /* Generate the filenames for the two output files. */
    snprintf (output_inp_filename, sizeof(output_inp_filename), "%s.inp", dataset);
    snprintf (output_pot_filename, sizeof(output_pot_filename), "%s.pot", dataset);

    /* Open the output files for writing. */
    output_inp_file = fopen_checked (output_inp_filename, "w", "input file");
    output_pot_file = fopen_checked (output_pot_filename, "w", "potential file");

    /* Write the output files and check for success. */
    if (write_inp_file (output_inp_file, mode)
        || write_pot_file (output_pot_file, params, mode)) {
        ret = EXIT_FAILURE;
    } else {
        /* Print out the name of the dataset so bash scripts can use it. */
        puts (dataset);
    }

    /* Handle the cleanup operations. */
    fclose (output_inp_file);
    fclose (output_pot_file);
    if (mode == ITERATING)
        fclose (input_pot_new_file);
    free (params);

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
    "LATTICE PARAMETERS\n"

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

/* Calculate the Wigner-Seitz radius of sites in a lattice, assuming that they
 * all share the same radius.  We pass the 2D matrix like this to make the
 * semantics clearer than const double **lat_matrix. */
double
wigner_seitz_radius (double lattice_parameter, const double lat_matrix[3][3], int nsites)
{
    double determinant = 0.0;
    determinant += lat_matrix[0][0] * (lat_matrix[1][1] * lat_matrix[2][2]
                                       - lat_matrix[1][2] * lat_matrix[2][1]);
    determinant += lat_matrix[0][1] * (lat_matrix[1][2] * lat_matrix[2][0]
                                       - lat_matrix[1][0] * lat_matrix[2][2]);
    determinant += lat_matrix[0][2] * (lat_matrix[1][0] * lat_matrix[2][1]
                                       - lat_matrix[1][1] * lat_matrix[2][0]);
    const double volume = determinant * CUBE(lattice_parameter);
    return cbrt (0.75 * volume / (M_PI * (double) nsites));
}
