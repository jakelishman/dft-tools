#!/bin/bash

# SYNOPSIS
#   scf-run.sh [-t] generator [outdir]
#
#   Reads sets of generator arguments in from stdin, then organises them in such
#   a way that the run is submitted safely by qsub.
#
# ARGUMENTS
#   -t
#       Test run - perform all logic, but leave the script file in place and do
#       not submit to PBS.  If not given, the run will be qsubbed.
#
#   generator
#       A generator program which, given a line from stdin, will create two
#       files in the current directory, one which is an input file for KKRSCF
#       and one which is a corresponding potential file.  The generator should
#       also output to stdout (only this line) a "dataset" string, which should
#       be the non-extension part of the input and potential filenames.  This
#       should be unique to the input parameters.
#
#       The two filenames should be respectively "${dataset}.inp" and
#       "${dataset}.pot", where ${dataset} is the string the generator printed
#       to stdout.
#
#       Example generator sources are in gen/, in the root directory of this git
#       repository.
#
#   outdir
#       The directory to place all output files in.  All work will be done on
#       local CSC machines in the /tmp directory, but data will be copied back
#       to outdir.  Assumed to be . if not given.
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

# Set a number of operations to do per CoW task so we have to wait less!
chunksize=32
chunkcount=$((count / chunksize))

# Account for any remainder.
if [ "$((count % chunksize))" -ne "0" ]; then
    chunkcount=$((chunkcount + 1))
fi

# Check we've at least got something to qsub.
if [ $count -eq 0 ]; then
    rm ${inputs}
    echo "Error: must have at least one set of lattice parameters!" >&2
    exit 1
fi

# The output script that we will be qsub-ing.
script=scf-qsubber.sh

# The kkrscf binary.
bin=${HOME}/bin/kkrscf5.4

# Actually write the script into the correct place.
cat > ${script} << ScriptEnd
#!/bin/bash
#PBS -q taskfarm
#PBS -l nodes=1:ppn=1,walltime=48:00:00
#PBS -t 1-${chunkcount}
#PBS -V

# Make the temporary directory so writing the new potentials doesn't hit CSC
# networked storage.
tempdir=\$(mktemp -d)
cd \${tempdir}

# Calculate the first line this array instance will want from the input
# parameters file.
first=\$(((PBS_ARRAYID - 1) * ${chunksize} + 1))

# Calculate the last line this array instance will want from the input
# parameters file - if this is the last in the array, then we just want to get
# all remaining parameters.
if [ \${PBS_ARRAYID} -ne '${chunkcount}' ]; then
    last=\$((PBS_ARRAYID * ${chunksize}))
else
    last=${count}
fi

# Loop through all the lattice parameters we want to do in this run.
for i in \`seq \${first} \${last}\`; do
    dataset=\`tail -n+\${i} "\${PBS_O_WORKDIR}/${inputs}" | head -n1 | xargs ${gen} \`
    ${bin} < \${dataset}.inp > \${dataset}.out

    # Copy the created files back to networked storage as we go in case the job
    # is killed.
    cp -r ./\${dataset}* \${PBS_O_WORKDIR}
done

# Get rid of the temporary directory.
cd \${PBS_O_WORKDIR}
rm -rf \${tempdir}
ScriptEnd

# Qsub if it's not a test run.
if [ "${testrun}" = false ]; then
    # Load the modules we need for `kkrscf` to run. 
    module load intel/13.1
    module load ompi/1.6.4/intel/13.1

    # qsub the job!
    qsub ${script}

    # Get rid of the temporary script.
    rm ${script}
fi
