#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

bool
correct_number_of_parameters (int nparams)
{
    return nparams == 1;
}

void
specific_usage (void)
{
    fprintf (stderr, "Takes only one lattice parameter, which should be the same as the lattice parameter\n");
    fprintf (stderr, "of CsCl FeRh.\n");
    return;
}

void
make_dataset (const double *params)
{
    snprintf (dataset, sizeof(dataset), "%.10g", params[0]);
}

int
write_inp_file (FILE *out, int mode)
{
    (void) mode;

    fprintf (out, "CONTROL  DATASET     = %s\n", dataset);
    fprintf (out, "         ADSI        = SCF\n");
    fprintf (out, "         POTFIL      = %s\n", output_pot_filename);
    fprintf (out, "         PRINT = 0\n");
    fprintf (out, "\n");
    fprintf (out, "MODE     SP-SREL\n");
    fprintf (out, "\n");
    fprintf (out, "TAU      BZINT= POINTS  NKTAB= 250\n");
    fprintf (out, "\n");
    fprintf (out, "ENERGY   GRID={5}  NE={30}\n");
    fprintf (out, "         EMIN=-0.2   ImE=0.0 Ry\n");
    fprintf (out, "\n");
    fprintf (out, "SCF      NITER=200 MIX=0.10 VXC=VWN\n");
    fprintf (out, "         TOL=0.00001  MIXOP=0.10  ISTBRY=1\n");
    fprintf (out, "         QIONSCL=1.0\n");
    fprintf (out, "         MSPIN={3.0, 1.0, 3.0, 1.0}\n");
    fprintf (out, "         NOSSITER\n");

    return 0;
}

int
write_pot_file (FILE *out, const double *params, int mode)
{
    (void) mode;

    const double rws = 0.49237251092 * params[0];
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
    fprintf (out, "NQ                 4\n");
    fprintf (out, "NT                 4\n");
    fprintf (out, "NM                 2\n");
    fprintf (out, "IREL               2\n");
    fprintf (out, "*******************************************************************************\n");
    fprintf (out, "SCF-INFO\n");
    fprintf (out, "INFO      NONE\n");
    fprintf (out, "SCFSTATUS START\n");
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
    fprintf (out, "SCF-ITER           0\n");
    fprintf (out, "SCF-MIX       0.1000000000\n");
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
    fprintf (out, "ALAT          %.10f\n", 2 * params[0]);
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

    return 0;
}
