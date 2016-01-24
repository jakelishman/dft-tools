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

if [ $# -ne 5 ]; then
    echo "Usage: scf-run.sh outdir generator a_0 da a_n" >&2
    exit 1
fi

# Rename the input variables to usable names.
outdir=$1
first=$3
inc=$4
last=$5
gen=`which $2 2>/dev/null`

# Check that the generator exists in the path
if [ $? -ne 0 ]; then
    echo "Error: could not find the generator \`$2\` in the path." >&2
    exit 1
fi

# Check that the increment is not equal to 0
if [ `echo "${inc} == 0.0" | bc` -eq 1 ]; then
    echo "Error: the increment cannot be 0." >&2
    exit 1
fi

# Make the output directory if it doesn't exist.
mkdir -p ${outdir}

# Calculate the number of steps we were asked for, making sure to account for
# floating point errors.  Integer division by 1 handles the rounding down.
count=`echo "(((${last} - ${first}) / ${inc}) + 0.5 * ${inc} + 1) / 1" | bc`

# The output script that we will be qsub-ing.
local_script=scf-out.sh
script=${outdir}/${local_script}

# The kkrscf binary.
bin=${HOME}/bin/kkrscf5.4

# Actually write the script into the correct place.
echo "#!/bin/bash" > ${script}
echo "cd \$PBS_O_WORKDIR" >> ${script}
echo "lattice=\`echo \"(\${PBS_ARRAYID} - 1) * ${inc} + ${first}\" | bc\`" >> ${script}
echo "${gen} \${lattice}" >> ${script}
echo "${bin} < \`printf "%f.inp" \${lattice}\` > \`printf "%f.out" \${lattice}\`" >> ${script}

# Save our current directory.
startdir=${PWD}

# Change to the output directory so all our temporary files end up in the right
# place.
cd ${outdir}

# Load the modules we need for `kkrscf` to run. 
module load intel/13.1
module load ompi/1.6.4/intel/13.1

# qsub the job!
qsub -q taskfarm -l nodes=1:ppn=1,walltime=04:00:00 -t 1-${count} -V ${local_script}

# Go back to where we started.
cd ${startdir}

# Get rid of the temporary script.
rm ${script}
