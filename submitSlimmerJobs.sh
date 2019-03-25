#!/bin/bash

date

echo "SUBMITTING nominal"

python -u runCondorSlimmerJobs.py nominal

sleep 5

echo "SUBMITTING JECup"

python -u runCondorSlimmerJobs.py JECup

sleep 5

echo "SUBMITTING JECdown"

python -u runCondorSlimmerJobs.py JECdown

sleep 5

echo "SUBMITTING JERup"

python -u runCondorSlimmerJobs.py JERup

sleep 5

echo "SUBMITTING JERdown"

python -u runCondorSlimmerJobs.py JERdown

sleep 5

echo "SUBMITTING BTAGup"

python -u runCondorSlimmerJobs.py BTAGup

sleep 5

echo "SUBMITTING BTAGdown"

python -u runCondorSlimmerJobs.py BTAGdown

sleep 5

echo "SUBMITTING LTAGup"

python -u runCondorSlimmerJobs.py LTAGup

sleep 5

echo "SUBMITTING LTAGdown"

python -u runCondorSlimmerJobs.py LTAGdown

sleep 5

echo "DONE"

date

