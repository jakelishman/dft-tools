#!/bin/bash

# SYNOPSIS
#   scf-run-serial.sh generator [outdir]
#   and pipe lattice parameters into the script.
#
# BUGS
#   Lattice parameters piped into the program must be \n-separated, not
#   space-separated.  The script fails noisily, but not gracefully if the latter
#   is given.
# AUTHOR
#   Jake Lishman
# EMAIL
#   j.lishman@warwick.ac.uk
# DATE
#   2016-01-28

# If we're not given the generator, then we must fail.
if [ $# -ne 1 ] && [ $# -ne 2 ]; then
    echo "Usage: scf-run-serial.sh generator [outdir]" >&2
    echo "       and pipe in lattice parameters" >&2
    exit 1
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

# Read in the first lattice parameter.
IFS=
read -r first

# Initialise the other lattice parameter string.
otherlats=""

# Read in other lattice paramters.
while IFS= read -r curlat; do
    # If nothing was read in, then end.
    [[ $curlat ]] || break

    # Add the next line to the lattice parameter, trimming trailing space.
    otherlats+=`echo "$curlat" | xargs`
    # Add a single space to separate.
    otherlats+=" "
done

# Trim final space.
otherlats=`echo "$otherlats" | xargs`

# The output script that we will be qsub-ing.
script="scf${first}-${otherlats##* }.sh"

# The kkrscf binary.
bin=${HOME}/bin/kkrscf5.4

# Load the modules we need for `kkrscf` to run. 
module load intel/13.1
module load ompi/1.6.4/intel/13.1

# Actually write the script into the correct place.
echo '#!/bin/bash' > ${script}
echo 'cd ${PBS_O_WORKDIR}' >> ${script}
echo "${gen} -s ${first}" >> ${script}
echo "${bin} < `printf '%f.inp' ${first}` > `printf '%f.out' ${first}`" >> ${script}
echo "prevf=`printf '%f' ${first}`" >> ${script}

# If there are other lattice parameters to write, then we need to write their
# loop.  Still works even if there's only one other lattice parameter..
if [ -n "$otherlats" ]; then 
    echo "for i in ${otherlats}; do" >> ${script}
    echo '    latf=`printf "%f" ${i}`' >> ${script}
    echo "    ${gen} -i \${i} \${prevf}.pot_new" >> ${script}
    echo "    ${bin} < \${latf}.inp > \${latf}.out" >> ${script}
    echo '    prevf=${latf}' >> ${script}
    echo 'done' >> ${script}
fi

# qsub the job!
qsub -l nodes=1:ppn=1,walltime=72:00:00 -V ${script}

# Get rid of the temporary script.
rm ${script}
