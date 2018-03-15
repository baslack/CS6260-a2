#!/bin/sh -login
WAIT=$1;
for t in 1 2 4 8 16;
do
	for n in 2500 3536 5000 7071 10000;
	do
		if [ $t -eq 1 ]
		then
			echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -s 1\" a2_otr.pbs";
			qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -s 1" a2_otr.pbs;
			sleep ${WAIT};
		else		
			for chunk in 64 128 256 512 1024;
			do
				echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -s ${chunk}\" a2_otr.pbs";
				qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -s ${chunk}" a2_otr.pbs;
				sleep ${WAIT};
				
				echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -d ${chunk}\" a2_otr.pbs";
				qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -d ${chunk}" a2_otr.pbs;
				sleep ${WAIT};
				
				echo "qsub -l nodes=1:ppn=${t} -F \"-n ${n} -t ${t} -g ${chunk}\" a2_otr.pbs";
				qsub -l nodes=1:ppn=${t} -F "-n ${n} -t ${t} -g ${chunk}" a2_otr.pbs;
				sleep ${WAIT};
			done
		fi
	done
done
