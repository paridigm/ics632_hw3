#!/bin/bash
#SBATCH -n 80   # Number of tasks
#SBATCH -c 1    # Number of cores per task
#SBATCH -N 4    # Number of nodes requested (MAX)
#SBATCH -t 300  # Runtime in minutes
#SBATCH -o job.out # Standard out goes to this file
#SBATCH -e job.err # Standard err goes to this file
#SBATCH --mail-type ALL
#SBATCH --mail-user tknappra@hawaii.edu
#SBATCH -p ics632.q
#SBATCH --account=ics_632

# setup vars
epochs=3                   # tests per thread amount (not needed in local testing)

num_bytes=1000000000

chunk_size_start=10000
chunk_size_mult=10
chunk_size_end=50000

RUN_BASICS=true

BATCH_n=80                  # tasks/processes (only thing that matters)

#
# SLURM:
#
# run: default_bcast
#	   naive_bcast
#	   asynchronous_pipelined_ring_bcast 
#
# byte buffer: 10^9 = 1000000000
#
# smaller byte buffer to start
# 
# run each with a certain amount of trials --> lets try 5?
#
#	--> run async_pipelined with trials for the various chunk sizes --> try some of the better oness
#

results_dir=results_cray

def_res=def_time.txt
naive_res=naive_time.txt
async_res=async_pipeline_time.txt

# make directory
mkdir -p $results_dir

# clear potential previous results
if [ "$RUN_BASICS" = true ] ; then
> $results_dir/$def_res
> $results_dir/$naive_res
fi
> $results_dir/$async_res


### compile ###
mpicc bcast_skeleton.c -o bcast_skeleton -D NUM_BYTES=$num_bytes

# BASICS
if [ "$RUN_BASICS" = true ] ; then

###### default broadcast ########
echo ""
echo "********************************************* default broadcast **************************************************"
echo ""
for ((j=0; j<epochs; j++))
do
	mpirun -np $BATCH_n ./bcast_skeleton default_bcast -c 1 > res.txt
	str_t=$(cat res.txt | grep "time:" | sed "s/^.*time:/time:/" | sed "s/time: //" | sed "s/ seconds//")
	echo -n "$str_t, " >> $results_dir/$def_res
done


###### naive broadcast ########
echo ""
echo "********************************************* naive broadcast **************************************************"
echo ""
for ((j=0; j<epochs; j++))
do
	mpirun -np $BATCH_n ./bcast_skeleton naive_bcast -c 1 > res.txt
	str_t=$(cat res.txt | grep "time:" | sed "s/^.*time:/time:/" | sed "s/time: //" | sed "s/ seconds//")
	echo -n "$str_t, " >> $results_dir/$naive_res
done

fi
# END BASICS


###### async pipelined ring broadcast ########
echo ""
echo "********************************************* async pipelined ring broadcast **************************************************"
echo ""

for ((i=chunk_size_start; i<=chunk_size_end; i=i*chunk_size_mult))
do

	echo -n "$i, " >> $results_dir/$async_res
	for ((j=0; j<epochs; j++))
	do
		mpirun -np $BATCH_n ./bcast_skeleton asynchronous_pipelined_ring_bcast -c $i > res.txt
		str_t=$(cat res.txt | grep "time:" | sed "s/^.*time:/time:/" | sed "s/time: //" | sed "s/ seconds//")
		echo -n "$str_t, " >> $results_dir/$async_res
	done
	echo "" >> $results_dir/$async_res
	
	# x5
	i_mid=$((i*5))
	if (( i_mid <= chunk_size_end )); then	
		echo -n "$i_mid, " >> $results_dir/$async_res
		for ((j=0; j<epochs; j++))
		do			
			mpirun -np $BATCH_n ./bcast_skeleton asynchronous_pipelined_ring_bcast -c $i_mid > res.txt
			str_t=$(cat res.txt | grep "time:" | sed "s/^.*time:/time:/" | sed "s/time: //" | sed "s/ seconds//")
			echo -n "$str_t, " >> $results_dir/$async_res
		done
		echo "" >> $results_dir/$async_res
	fi
	
done


# log everything
cat $results_dir/$def_res
echo ""
cat $results_dir/$naive_res
echo ""
cat $results_dir/$async_res
echo ""
