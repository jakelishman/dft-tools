#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#define WS_RATIO 0.3907963208
#define MAX_FILENAME 1024

int write_pot_file (const double lattice_param);
int write_inp_file (const double lattice_param);

void make_pot_filename (char *filename, const double param);

int main (int argc, char **argv)
{
    int i, ret = 0;
    double lattice_param;
    char *test = argv[0];

    for (i = 1; i < argc; i++) {
        lattice_param = strtod (argv[i], &test);

        if (test == argv[i]) {
            fprintf (stderr, "Failed to convert lattice parameter %s.\n", argv[i]);
            ret = 1;
            continue;
        }

        if (write_pot_file (lattice_param)) {
            fprintf (stderr, "Couldn't write potential file for %g.\n", lattice_param);
            ret = 1;
            continue;
        }

        if (write_inp_file (lattice_param)) {
            fprintf (stderr, "Couldn't write input file for %g.\n", lattice_param);
            ret = 1;
            continue;
        }
    }

    return ret;
}

void make_pot_filename (char *filename, const double param)
{
    sprintf (filename, "%f.pot", param);
}

void make_inp_filename (char *filename, const double param)
{
    sprintf (filename, "%f.inp", param);
}

int write_inp_file (const double lattice_param)
{
    char pot_filename[MAX_FILENAME];
    char inp_filename[MAX_FILENAME];

    make_pot_filename (pot_filename, lattice_param);
    make_inp_filename (inp_filename, lattice_param);
    FILE *out = fopen (inp_filename, "w");
    if (!out) return 1;

    fprintf (out, "CONTROL  DATASET     = %f\n", lattice_param);
    fprintf (out, "         ADSI        = SCF\n");
    fprintf (out, "         POTFIL      = %s\n", pot_filename);
    fprintf (out, "         PRINT = 0\n");
    fprintf (out, "         NONMAG\n");
    fprintf (out, "\n");
    fprintf (out, "MODE     SREL\n");
    fprintf (out, "\n");
    fprintf (out, "TAU      BZINT= POINTS  NKTAB= 250\n");
    fprintf (out, "\n");
    fprintf (out, "ENERGY   GRID={5}  NE={30}\n");
    fprintf (out, "         EMIN=-0.2   ImE=0.0 Ry\n");
    fprintf (out, "\n");
    fprintf (out, "SCF      NITER=200 MIX=0.20 VXC=VWN\n");
    fprintf (out, "         TOL=0.00001  MIXOP=0.20  ISTBRY=1\n");
    fprintf (out, "         QIONSCL=1.0\n");
    fprintf (out, "         NOSSITER\n");

    fclose (out);
    return 0;
}

int write_pot_file (const double lattice_param)
{
    char pot_filename[MAX_FILENAME];
    make_pot_filename (pot_filename, lattice_param);

    FILE *out = fopen (pot_filename, "w");
    if (!out) return 1;

    const double rws = 0.49237251092 * lattice_param;
    const double rmt = 0.85 * rws;
    const double dx  = log (rws / 0.000001) / 720.0;

    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "HEADER    'SCF-start data created by xband  Thu Nov  5 10:29:44 GMT 2015'\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "TITLE     'SPR-KKR calculation for FeRh'\n");
    fprintf (out, "SYSTEM    FeRh\n");
    fprintf (out, "PACKAGE   SPRKKR\n");
    fprintf (out, "FORMAT    6  (21.05.2007)\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "GLOBAL SYSTEM PARAMETER\n");
    fprintf (out, "NQ                 2\n");
    fprintf (out, "NT                 2\n");
    fprintf (out, "NM                 2\n");
    fprintf (out, "IREL               1\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "SCF-INFO\n");
    fprintf (out, "INFO      NONE\n");
    fprintf (out, "SCFSTATUS START\n");
    fprintf (out, "FULLPOT   F\n");
    fprintf (out, "BREITINT  F\n");
    fprintf (out, "NONMAG    T\n");
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
    fprintf (out, "SCF-ITER           0\n");
    fprintf (out, "SCF-MIX       0.2000000000\n");
    fprintf (out, "SCF-TOL       0.0000100000\n");
    fprintf (out, "RMSAVV    999999.0000000000\n");
    fprintf (out, "RMSAVB    999999.0000000000\n");
    fprintf (out, "EF            0.0000000000\n");
    fprintf (out, "VMTZ          0.0000000000\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "LATTICE\n");
    fprintf (out, "SYSDIM       3D\n");
    fprintf (out, "SYSTYPE      BULK\n");
    fprintf (out, "BRAVAIS           12        cubic       primitive      m3m    O_h \n");
    fprintf (out, "ALAT          %.10f\n", lattice_param);
    fprintf (out, "A(1)          1.0000000000    0.0000000000    0.0000000000\n");
    fprintf (out, "A(2)          0.0000000000    1.0000000000    0.0000000000\n");
    fprintf (out, "A(3)          0.0000000000    0.0000000000    1.0000000000\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "SITES\n");
    fprintf (out, "CARTESIAN T\n");
    fprintf (out, "BASSCALE      1.0000000000    1.0000000000    1.0000000000\n");
    fprintf (out, "        IQ      QX              QY              QZ\n");
    fprintf (out, "         1    0.0000000000    0.0000000000    0.0000000000\n");
    fprintf (out, "         2    0.5000000000    0.5000000000    0.5000000000\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "OCCUPATION\n");
    fprintf (out, "        IQ     IREFQ       IMQ       NOQ  ITOQ  CONC\n");
    fprintf (out, "         1         1         1         1     1 1.000\n");
    fprintf (out, "         2         2         2         1     2 1.000\n");
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
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "MESH INFORMATION\n");
    fprintf (out, "MESH-TYPE EXPONENTIAL\n");
    fprintf (out, "   IM      R(1)            DX         JRMT      RMT        JRWS      RWS\n");
    fprintf (out, "    1    0.0000010000    %.10f    0    %.10f  721    %.10f\n", dx, rmt, rws);
    fprintf (out, "    2    0.0000010000    %.10f    0    %.10f  721    %.10f\n", dx, rmt, rws);
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "TYPES\n");
    fprintf (out, "   IT     TXTT        ZT     NCORT     NVALT    NSEMCORSHLT\n");
    fprintf (out, "    1     Fe              26        18         8              0\n");
    fprintf (out, "    2     Rh              45        36         9              0\n");

    fclose (out);
    return 0;
}
