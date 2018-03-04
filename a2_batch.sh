#!/bin/sh -login
WAIT=$1;
for t in 1 2 4 8 16;
do
	for n in 1000 1260 1587 2000 2520;
	do
		for chunk in 1 4 16 64;
		do
			echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -s ${chunk}\" a2.pbs";
			qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -s ${chunk}" a2.pbs;
			sleep ${WAIT};
			
			echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -d ${chunk}\" a2.pbs";
			qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -d ${chunk}" a2.pbs;
			sleep ${WAIT};
			
			echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -g ${chunk}\" a2.pbs";
			qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -g ${chunk}" a2.pbs;
			sleep ${WAIT};
		done
	done
done
