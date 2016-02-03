#ifndef __DFT_GENERATOR_H
#define __DFT_GENERATOR_H

#include <stdbool.h>
#include <stdio.h>

/* Array size for the maximum filename. */
#define MAX_FILENAME 4096
#define MAX_DATASET  4000

enum _mode {
    START = 0,
    ITERATING = 1
};

int seek_to_line (FILE *file, const char *str);
int copy_lines   (FILE *out, FILE *in, long count);
int skip_lines   (FILE *in, long count);

bool correct_number_of_parameters (int nparams);

void make_dataset (const double *params);

int write_inp_file (FILE *out, int mode);
int write_pot_file (FILE *out, const double *params, int mode);

void specific_usage (void);

extern char dataset[MAX_FILENAME];
extern char output_inp_filename[MAX_FILENAME];
extern char output_pot_filename[MAX_FILENAME];

#endif
