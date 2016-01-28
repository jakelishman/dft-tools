#!/bin/bash

# SYNOPSIS
#   scf-run-serial.sh a0 [a1 ... an]
#   or pipe lattice parameters into the script.
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

# If there are no command line arguments then we need to read them from a pipe.
if [ $# -eq 0 ]; then
    # If stdin (&0) is a tty, then exit because something's gone wrong.
    if [ -t 0 ]; then
        echo "Usage: scf-run-serial.sh a0 [a1 ... an]" >&2
        echo "       or pipe lattice parameters into me." >&2
        exit 1
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

    # Trim that last trailing space off again.
    otherlats=`echo "$otherlats" | xargs`
else
    # Get the first command line argument.
    first=$1

    # Shift to ignore the first argument, then take the rest of them as the
    # string of arguments.
    shift
    otherlats=$@
fi

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
echo "${bin} < `printf '%f.inp' ${first}` > `printf '%f.out' ${first}`" >> ${script}
echo "prevf=`printf '%f' ${first}`" >> ${script}

# If there are other lattice parameters to write, then we need to write their
# loop.  Still works even if there's only one other lattice parameter..
if [ -n "$otherlats" ]; then 
    echo "for i in ${otherlats}; do" >> ${script}
    echo '    latf=`printf "%f" ${i}`' >> ${script}
    echo '    sed "s/${prevf}/${latf}/g" ${prevf}.inp > ${latf}.inp' >> ${script}
    echo '    sed "s/ALAT[[:space:]]*[0-9]\.[0-9]*E[-+][0-9]*$/ALAT              ${latf}/" ${prevf}.pot_new > ${latf}.pot' >> ${script}
    echo "    ${bin} < \${latf}.inp > \${latf}.out" >> ${script}
    echo '    prevf=${latf}' >> ${script}
    echo 'done' >> ${script}
fi

# qsub the job!
qsub -l nodes=1:ppn=1,walltime=72:00:00 -V ${script}

# Get rid of the temporary script.
rm ${script}
