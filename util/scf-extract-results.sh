#!/bin/bash
#PBS -l nodes=1:ppn=1,walltime=08:00:00

# NAME
#   scf-extract-results.sh - extract magnetisation and total energy levels from
#                            output files generated by kkrscf into CSV files
# 
# SYNOPSIS
#   scf-extract-results.sh folder1 [folder2 [...]]
#   qsub scf-extract-results.sh -F "folder1 [folder2 [...]]"
#
# DESCRIPTION
#   Bash script for extracting lattice parameters, magnetisation and total
#   integrated energy from output files generated by the `kkrscf` program from
#   the SPR-KKR package.  The lattice parameter is extracted from the file name
#   (we expect names in the format "6.000000.out", where "6.000000" can be
#   replaced by the relevant parameter.  We expect `kkrscf` to have been
#   invoked using the also supplied `scf-run.sh` script, which automatically
#   generates all the necessary files.
#
#   Prints results to standard output - we expect you to redirect it to where
#   you want it.
#
# AUTHOR
#   Jake Lishman
# EMAIL
#   j.lishman@warwick.ac.uk
# DATE
#   2015-11-19
# LAST MODIFIED
#   2015-11-21

# If we're not running as a qsub'd job, then set the workdir to the current
# directory

# Checking to make sure the first argument is the location of the convergence checker program
check=`which $1 2>/dev/null`

if [ $? -ne 0 ]; then
    echo "Error: could not find the cycle convergence checker \`$1\` in the path." >&2
    exit 1
fi

shift

if [ -z $PBS_O_WORKDIR ]; then
    PBS_O_WORKDIR=$PWD
fi

# Make sure we're in the right directory.
cd ${PBS_O_WORKDIR}

# Check that there's at least one folder to work with.
if [ $# -lt 1 ]; then
    echo "I need at least one folder to operate on!" >&2
    exit 1
fi

# Print out column names
echo "# param1, ..., mag, e_f, e_tot"

# Loop through all folders passed.
while [ $# -ge 1 ]; do

    # Check that the argument is actually a folder.
    if [ ! -d "${PBS_O_WORKDIR}/$1" ]; then
        echo "$1 is not a directory." >&2

        # Shift the arguments so we move to the next folder.
        shift
        continue
    fi

    # Add a comment to the results file telling us where the data is coming
    # from.
    echo "# Data folder: $1"

    # Make sure we're in the right directory.
    cd ${PBS_O_WORKDIR}/$1

    # Loop through the output files in the directory.
    for f in *_SCF.log; do
        # Get the lattice parameter (the first part of the filename).
        dataset=${f%_SCF.log}

        # Running the convergence checker program for this output file
        ${check} ${dataset}.out
        check_status=$?

        # If the convergence checker returned an error code, that means we didn't find the term
        # "cycle converged", so it didn't!
        if [ ${check_status} -eq 3 ]; then
            echo "For $1/${dataset}, error reading output file" >&2
        elif [ ${check_status} -eq 1 ]; then
            echo "$1/${dataset} did not converge" >&2
        # Otherwise, the cycle converged and we can print out the results.
        else
            line=$(echo `tail -n 1 ${f}`)
            arr=($line)

            params=`echo "${dataset}" | sed 's/-/, /'`

            # Print out the lattice parameter, magnetisation, Fermi energy and
            # total integrated energy, in that order.
            printf "%s, %.15g, %.15g, %.15g\n" "${params}" "${arr[7]}" "${arr[4]}" "${arr[9]}"
        fi
    done

    # Move to the next folder in the argument list.
    shift
done
