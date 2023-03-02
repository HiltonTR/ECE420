#! /bin/bash
# 
# 
# Usage:
#	./test.sh
# Notes:
#


# remove previously generated files
rm -f *_nt_*
clear

# Parameters
Duplicates=100
num_threads=(5 10 50 100)
mat_sizes=(20 100 500)
prog_names=(./steph)

# do the stuff
echo "Start..."

for ms in "${mat_sizes[@]}"
do
echo "Starting trials for matrices of size ${ms}..."
	for nt in "${num_threads[@]}"
	do
		echo "   with ${nt} threads..."
		trial=0
		while [ $trial -ne $Duplicates ]
		do
			let trial+=1
			./datagen $ms
				for prog in "${prog_names[@]}"
				do
					$prog $nt $ms
				done
		done
	done
done

echo "Done"
