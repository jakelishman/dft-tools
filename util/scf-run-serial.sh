#!/bin/bash

# SYNOPSIS
#   scf-run-serial.sh [-t] generator [outdir]
#   and pipe lattice parameters into the script.
#
# AUTHOR
#   Jake Lishman
# EMAIL
#   j.lishman@warwick.ac.uk
# DATE
#   2016-01-28

# If we're not given the generator, then we must fail.
if [ $# -lt 1 ] || [ $# -gt 3 ]; then
    echo "Usage: scf-run-serial.sh [-t] generator [outdir]" >&2
    echo "       and pipe in a list of arguments to the generator, one set per line" >&2
    exit 1
fi

# Check to see if this is a test run (whether or not we should actually do the
# qsub'ing and rm'ing the script at the end.
if [ "$1" = "-t" ]; then
    testrun=true
    shift
else
    testrun=false
fi

# Get the absolute path of the generator.
gen=`which $1 2>/dev/null`

# Check that the generator exists in the path
if [ $? -ne 0 ]; then
    echo "Error: could not find the generator '$1' in the path." >&2
    exit 1
fi

# Change into the output directory, if it exists.
if [ -n "$2" ]; then
    mkdir -p "$2"
    if [ $? -ne 0 ]; then
        echo "Error: could not create directory '$2'." >&2
    fi
    cd "$2"
fi

# The output script that we will be qsub-ing.
script=`date +"scf-qsubber.sh%s%N"`

# The kkrscf binary.
bin=${HOME}/bin/kkrscf5.4

if [ "${testrun}" = false ]; then
    # Load the modules we need for `kkrscf` to run.
    module load intel/13.1
    module load ompi/1.6.4/intel/13.1
fi

IFS=
read -r curargs

if [ -z "${curargs}" ]; then
    echo "Need at least one set of lattice parameters!" >&2
    exit 1
fi

# Set up the initial part of the script.
echo '#!/bin/bash' > ${script}
echo 'cd ${PBS_O_WORKDIR}' >> ${script}
echo "dataset=\`${gen} -s ${curargs}\`" >> ${script}
echo "${bin} < \${dataset}.inp > \${dataset}.out" >> ${script}
echo 'prev=${dataset}' >> ${script}

# Read in the rest of the lattice parameter arguments.
while IFS= read -r curargs; do
    # If nothing was read in, then end.
    [[ $curargs ]] || break

    echo "dataset=\`${gen} -i \"\${prev}.pot_new\" ${curargs}\`" >> ${script}
    echo "${bin} < \${dataset}.inp > \${dataset}.out" >> ${script}
done

if [ "${testrun}" = false ]; then
    # qsub the job!
    qsub -l nodes=1:ppn=1,walltime=72:00:00 -V ${script}

    # Get rid of the temporary script.
    rm ${script}
fi
