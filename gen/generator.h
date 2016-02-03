#ifndef __DFT_GENERATOR_H
#define __DFT_GENERATOR_H

#include <stdbool.h>
#include <stdio.h>

/* Array size for the maximum filename. */
#define MAX_FILENAME 4096
#define MAX_DATASET  4000

/* A square macro for more efficient squaring. */
#define SQUARE(a) ((a) * (a) * (a))

/* A cube macro for more efficient cubing. */
#define CUBE(a) ((a) * (a) * (a))

enum _mode {
    START = 0,
    ITERATING = 1
};

int seek_to_line (FILE *file, const char *str);
int copy_lines   (FILE *out, FILE *in, long count);
int skip_lines   (FILE *in, long count);

double wigner_seitz_radius (double lattice_param, const double lat_matrix[3][3], int nsites);

bool correct_number_of_parameters (int nparams);

void make_dataset (const double *params);

int write_inp_file (FILE *out, int mode);
int write_pot_file (FILE *out, const double *params, int mode);

void specific_usage (void);

extern char dataset[MAX_FILENAME];
extern char output_inp_filename[MAX_FILENAME];
extern char output_pot_filename[MAX_FILENAME];

#endif
