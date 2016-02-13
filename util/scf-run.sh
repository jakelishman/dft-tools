#!/bin/bash

# SYNOPSIS
#   scf-run.sh outdir generator a_0 da a_n
#
# AUTHOR
#   Jake Lishman
# EMAIL
#   j.lishman@warwick.ac.uk
# DATE
#   2016-01-19

# If we're not given the generator, then we must fail.
if [ $# -lt 1 ] && [ $# -gt 3 ]; then
    echo "Usage: scf-run.sh [-t] generator [outdir]" >&2
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
    outdir="$2"
    mkdir -p "${outdir}"
    if [ $? -ne 0 ]; then
        echo "Error: could not create directory '${outdir}'." >&2
        exit 1
    fi
    cd "${outdir}"
fi

# Make the directory we need for our files.
scfdir=".scf"
mkdir -p ${scfdir}
if [ $? -ne 0 ]; then
    echo "Error: could not create directory '${scfdir}'." >&2
    exit 1
fi

# Create variables of the temporary files we'll be using.
prefix=`date +%s_`
inputs="${scfdir}/${prefix}inputs"

# Redirect the piped in input to the inputs file.
$(cat - > ${inputs})

# Get the number of inputs.
count=`wc -l ${inputs}` 
count=${count% *}

# Check we've at least got something to qsub.
if [ $count -eq 0 ]; then
    #rm ${inputs}
    echo "Error: must have at least one set of lattice parameters!" >&2
    exit 1
fi

# The output script that we will be qsub-ing.
script=scf-qsubber.sh

# The kkrscf binary.
bin=${HOME}/bin/kkrscf5.4

# Actually write the script into the correct place.
echo "#!/bin/bash" > ${script}
echo 'tempdir=$(mktemp -d)' >> ${script}
echo 'cd ${tempdir}' >> ${script}
echo "dataset=\`tail -n+\${PBS_ARRAYID} \"\${PBS_O_WORKDIR}/${inputs}\" | head -n 1 | xargs ${gen}\`" >> ${script}
echo "${bin} < \${dataset}.inp > \${dataset}.out" >> ${script}
echo 'cp -r ./* ${PBS_O_WORKDIR}/' >> ${script}
echo 'rm -rf ${tempdir}' >> ${script}

if [ "${testrun}" = false ]; then
    # Load the modules we need for `kkrscf` to run. 
    module load intel/13.1
    module load ompi/1.6.4/intel/13.1

    # qsub the job!
    qsub -q taskfarm -l nodes=1:ppn=1,walltime=04:00:00 -t 1-${count} -V ${script}

    # Get rid of the temporary script.
    rm ${script}
fi
