#!/bin/bash
qstat -u $(whoami) | tail -n+6 | sed 's/\..*//'
