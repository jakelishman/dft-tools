#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h> 
#define MAX_FILENAME 1024
#define LINEBUF 4096

void write_pot_file (int mode);
void write_inp_file (int mode);

char pot_filename[MAX_FILENAME];
char inp_filename[MAX_FILENAME];

void seek_to_line (FILE *file, const char *str);
void copy_lines (FILE *out, FILE *in, long count);
void skip_lines (FILE *in, long count);

char *pot_new_file;
double dx, rmt, rws, lattice_param;

enum _mode {
    START = 0,
    ITERATING = 1
};

int main (int argc, char **argv)
{
    int mode;
    char *test = argv[0];

    if (argc == 4 && !strcmp (argv[1], "-i")) {
        mode = ITERATING;
        pot_new_file = argv[3];
    } else if (argc == 3 && !strcmp (argv[1], "-s")) {
        mode = START;
    } else {
        fprintf (stderr, "Usage: gen [-s lattice] [-i lattice pot_new-file]\n");
        return 1;
    }

    lattice_param = strtod (argv[2], &test);
    if (test == argv[2]) {
        fprintf (stderr, "Failed to convert lattice parameter %s.\n", argv[2]);
        return 1;
    }

    sprintf (pot_filename, "%f.pot", lattice_param);
    sprintf (inp_filename, "%f.inp", lattice_param);

    rws = 0.49237251092 * lattice_param;
    rmt = 0.85 * rws;
    dx  = log (rws / 0.000001) / 720.0;

    write_pot_file (mode);
    write_inp_file (mode);

    return 0;
}

void
write_inp_file (int mode)
{
    FILE *out = fopen (inp_filename, "w");
    if (!out) {
        fprintf (stderr, "Couldn't write input file \"%s\"\n", inp_filename);
        exit (EXIT_FAILURE);
    }

    fprintf (out, "CONTROL  DATASET     = %f\n", lattice_param);
    fprintf (out, "         ADSI        = SCF\n");
    fprintf (out, "         POTFIL      = %s\n", pot_filename);
    fprintf (out, "         PRINT = 0\n");
    fprintf (out, "\n");
    fprintf (out, "MODE     SP-SREL\n");
    fprintf (out, "\n");
    fprintf (out, "TAU      BZINT= POINTS  NKTAB= 250\n");
    fprintf (out, "\n");
    fprintf (out, "ENERGY   GRID={5}  NE={30}\n");
    fprintf (out, "         EMIN=-0.2   ImE=0.0 Ry\n");
    fprintf (out, "\n");
    fprintf (out, "SCF      NITER=200 MIX=0.05 VXC=VWN\n");
    fprintf (out, "         TOL=0.00001  MIXOP=0.10  ISTBRY=1\n");
    fprintf (out, "         QIONSCL=1.0\n");
    if (mode == START) {
        fprintf (out, "         MSPIN={2.9, 0, -2.9, 0}\n");
    }
    fprintf (out, "         NOSSITER\n");

    fclose (out);
}

void
write_pot_file (int mode)
{
    FILE *in;
    if (mode == ITERATING) {
        in = fopen (pot_new_file, "r");
        if (!in) {
            fprintf (stderr, "Couldn't open pot_new filee \"%s\"\n", pot_new_file);
            exit (EXIT_FAILURE);
        }
    }
    FILE *out = fopen (pot_filename, "w");
    if (!out) {
        fprintf (stderr, "Couldn't open potential file \"%s\"\n", pot_filename);
        exit (EXIT_FAILURE);
    }

    const char *(header[2])    = {"SCF-start data created by xband  Thu Nov  5 10:29:44 GMT 2015",
                                  "SPR-KKR dataset created by KKRSCF"};
    const char *(title[2])     = {"SPR-KKR calculation for FeRh",
                                  "DOS for SCF-start"};
    const char *(info[2])      = {"NONE", "SRA-KKR-ASA"};
    const char *(scfstatus[2]) = {"START", "ITERATING"};

    if (mode == START) {
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "HEADER    '%s'\n", header[mode]);
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "TITLE     '%s'\n", title[mode]);
        fprintf (out, "SYSTEM    FeRh\n");
        fprintf (out, "PACKAGE   SPRKKR\n"); fprintf (out, "FORMAT    6  (21.05.2007)\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "GLOBAL SYSTEM PARAMETER\n");
        fprintf (out, "NQ                 4\n");
        fprintf (out, "NT                 4\n");
        fprintf (out, "NM                 2\n");
        fprintf (out, "IREL               2\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "SCF-INFO\n");
        fprintf (out, "INFO      %s\n", info[mode]);
        fprintf (out, "SCFSTATUS %s\n", scfstatus[mode]);
        fprintf (out, "FULLPOT   F\n");
        fprintf (out, "BREITINT  F\n");
        fprintf (out, "NONMAG    F\n");
        fprintf (out, "ORBPOL    NONE\n");
        fprintf (out, "EXTFIELD  F\n");
        fprintf (out, "BLCOUPL   F\n");
        fprintf (out, "BEXT          0.0000000000\n");
        fprintf (out, "SEMICORE  F\n");
        fprintf (out, "LLOYD     F\n");
        fprintf (out, "NE                30\n");
        fprintf (out, "IBZINT             2\n");
        fprintf (out, "NKTAB              0\n");
        fprintf (out, "XC-POT    VWN\n");
        fprintf (out, "SCF-ALG   BROYDEN2\n");
        fprintf (out, "SCF-ITER           %d\n", mode);
        fprintf (out, "SCF-MIX       0.0500000000\n");
        fprintf (out, "SCF-TOL       0.0000100000\n");
        fprintf (out, "RMSAVV    999999.0000000000\n");
        fprintf (out, "RMSAVB    999999.0000000000\n");
        fprintf (out, "EF            0.0000000000\n");
        fprintf (out, "VMTZ          0.0000000000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "LATTICE\n");
        fprintf (out, "SYSDIM       3D\n");
        fprintf (out, "SYSTYPE      BULK\n");
        fprintf (out, "BRAVAIS           13        cubic       face-centered  m3m    O_h \n");
        fprintf (out, "ALAT          %.10f\n", 2 * lattice_param);
        fprintf (out, "A(1)          0.0000000000    0.5000000000    0.5000000000\n");
        fprintf (out, "A(2)          0.5000000000    0.0000000000    0.5000000000\n");
        fprintf (out, "A(3)          0.5000000000    0.5000000000    0.0000000000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "SITES\n");
        fprintf (out, "CARTESIAN T\n");
        fprintf (out, "BASSCALE      1.0000000000    1.0000000000    1.0000000000\n");
        fprintf (out, "        IQ      QX              QY              QZ\n");
        fprintf (out, "         1    0.0000000000    0.0000000000    0.0000000000\n");
        fprintf (out, "         2    0.2500000000    0.2500000000    0.2500000000\n");
        fprintf (out, "         3    0.5000000000    0.5000000000    0.5000000000\n");
        fprintf (out, "         4    0.7500000000    0.7500000000    0.7500000000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "OCCUPATION\n");
        fprintf (out, "        IQ     IREFQ       IMQ       NOQ  ITOQ  CONC\n");
        fprintf (out, "         1         1         1         1     1 1.000\n");
        fprintf (out, "         2         1         1         1     2 1.000\n");
        fprintf (out, "         3         2         2         1     3 1.000\n");
        fprintf (out, "         4         2         2         1     4 1.000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "REFERENCE SYSTEM\n");
        fprintf (out, "NREF              2\n");
        fprintf (out, "      IREF      VREF            RMTREF\n");
        fprintf (out, "         1    4.0000000000    0.0000000000\n");
        fprintf (out, "         2    4.0000000000    0.0000000000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "MAGNETISATION DIRECTION\n");
        fprintf (out, "KMROT              0\n");
        fprintf (out, "QMVEC         0.0000000000    0.0000000000    0.0000000000\n");
        fprintf (out, "        IQ      QMTET           QMPHI \n");
        fprintf (out, "         1    0.0000000000    0.0000000000\n");
        fprintf (out, "         2    0.0000000000    0.0000000000\n");
        fprintf (out, "         3    0.0000000000    0.0000000000\n");
        fprintf (out, "         4    0.0000000000    0.0000000000\n");
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "MESH INFORMATION\n");
        fprintf (out, "MESH-TYPE EXPONENTIAL\n");
        fprintf (out, "   IM      R(1)            DX         JRMT      RMT        JRWS      RWS\n");
        fprintf (out, "    1    0.0000010000    %.10f    0    %.10f  721    %.10f\n", dx, rmt, rws);
        fprintf (out, "    2    0.0000010000    %.10f    0    %.10f  721    %.10f\n", dx, rmt, rws);
        fprintf (out, "*******************************************************************************\n");
        fprintf (out, "TYPES\n");
        fprintf (out, "   IT     TXTT        ZT     NCORT     NVALT    NSEMCORSHLT\n");
        fprintf (out, "    1     Fe_1            26        18         8              0\n");
        fprintf (out, "    2     Rh_1            45        36         9              0\n");
        fprintf (out, "    3     Fe_2            26        18         8              0\n");
        fprintf (out, "    4     Rh_2            45        36         9              0\n");
    } else {
        copy_lines (out, in, 44);
        fprintf (out, "ALAT        %.10f\n", 2 * lattice_param);
        skip_lines (in, 1);
        copy_lines (out, in, -1);
        fclose (in);
    }

    fclose (out);
}

void
seek_to_line (FILE *file, const char *str)
{
    char line[LINEBUF];
    while (fgets (line, LINEBUF, file)) {
        line [strlen (line) - 1] = '\0';
        if (!strcmp (line, str))
            return;
    }

    fprintf (stderr, "Couldn't find line \"%s\"\n", str);
    exit (EXIT_FAILURE);
}

void
copy_lines (FILE *out, FILE *in, long count)
{
    char line[LINEBUF];
    const int inf = (count < 0) ? 1 : 0;
    count = inf ? LONG_MAX : count;

    while (count-- && fgets (line, LINEBUF, in))
        fwrite (line, 1, strlen (line), out);

    if (!inf && count >= 0) {
        fprintf (stderr, "Tried to read %ld too many lines from pot_new file.\n", count + 1);
        exit (EXIT_FAILURE);
    }
    return;
}

void
skip_lines (FILE *in, long count)
{
    static char tmp[LINEBUF];

    if (count < 0) {
        fprintf (stderr, "Can't skip a negative number of lines!\n");
        exit (EXIT_FAILURE);
    }

    while (count-- && fgets (tmp, LINEBUF, in));

    if (count >= 0) {
        fprintf (stderr, "Tried to skip %ld too many lines from pot_new file.\n", count + 1);
        exit (EXIT_FAILURE);
    }

    return;
}
