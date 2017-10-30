// 
// smpicc bcast_skeleton.c -o bcast_skeleton
//
// smpirun --cfg=smpi/bcast:mpich -np 50 -platform ring_50.xml -hostfile hostfile_50.txt ./bcast_skeleton naive_bcast -c 1
//
// smpirun --cfg=smpi/bcast:mpich -np 50 -platform ring_50.xml -hostfile hostfile_50.txt ./bcast_skeleton ring_bcast -c 1
//
// c = chunk size 
//

// char *bcast_implementation_name:   the bcast implementation name (argument #1)
// int chunk_size:                    the chunk size (optional argument #2)
// int NUM_BYTES:                     the number of bytes to broadcast
// char *buffer:                      the buffer to broadcast


if( strcmp(bcast_implementation_name, "asynchronous_pipelined_ring_bcast") == 0 ) {
	
	if( 0 == rank ) {														// send to rank 1
		
		// debug
		//printf("rank %d sending to %d \n", rank, 1);
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			// printf( "%d to %d\n", i, i+chunk_size-1);
			// printf( "size %d\n", chunk_size);
			MPI_Send(buffer+i, chunk_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			// printf( "last %d to %d\n", i, NUM_BYTES-1);
			// printf( "size %d\n", NUM_BYTES-i);
			MPI_Send(buffer+i, NUM_BYTES-i, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		}
			
	}else if (num_procs-1 == rank) {										// last rank doesn't send
		
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			MPI_Recv(buffer+i, chunk_size, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			MPI_Recv(buffer+i, NUM_BYTES-i, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
	}else{
		
		char has_sent = 0;
		MPI_Status status;
		MPI_Request request = MPI_REQUEST_NULL;
		
		
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			MPI_Recv(buffer+i, chunk_size, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if(has_sent)
				MPI_Wait(&request, &status);
			else
				has_sent = 1;
			
			MPI_Isend(buffer+i, chunk_size, MPI_BYTE, rank+1, 0, MPI_COMM_WORLD, &request);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			MPI_Recv(buffer+i, NUM_BYTES-i, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			if(has_sent)
				MPI_Wait(&request, &status);
			else
				has_sent = 1;
			
			MPI_Isend(buffer+i, NUM_BYTES-i, MPI_BYTE, rank+1, 0, MPI_COMM_WORLD, &request);
			
		}
		
	}
	
}

if( strcmp(bcast_implementation_name, "pipelined_ring_bcast") == 0 ) {
	
	if( 0 == rank ) {														// send to rank 1
		
		// debug
		//printf("rank %d sending to %d \n", rank, 1);
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			// printf( "%d to %d\n", i, i+chunk_size-1);
			// printf( "size %d\n", chunk_size);
			MPI_Send(buffer+i, chunk_size, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			// printf( "last %d to %d\n", i, NUM_BYTES-1);
			// printf( "size %d\n", NUM_BYTES-i);
			MPI_Send(buffer+i, NUM_BYTES-i, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		}
			
	}else if (num_procs-1 == rank) {										// last rank doesn't send
		
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			MPI_Recv(buffer+i, chunk_size, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			MPI_Recv(buffer+i, NUM_BYTES-i, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		
	}else{
		
		for(i=0; i<NUM_BYTES-chunk_size+1; i+=chunk_size) {		// typical chunks
			MPI_Recv(buffer+i, chunk_size, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(buffer+i, chunk_size, MPI_BYTE, rank+1, 0, MPI_COMM_WORLD);
		}
		if(i<NUM_BYTES) {										// potential smaller chunk
			MPI_Recv(buffer+i, NUM_BYTES-i, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(buffer+i, NUM_BYTES-i, MPI_BYTE, rank+1, 0, MPI_COMM_WORLD);
		}
		
	}
	
}


if( strcmp(bcast_implementation_name, "default_bcast") == 0 ) {
	
	//MPI_Bcast(buffer ref, amt data, datatype, root, comm )
	MPI_Bcast(buffer, NUM_BYTES, MPI_BYTE, 0, MPI_COMM_WORLD);
	
}


if( strcmp(bcast_implementation_name, "ring_bcast") == 0 ) {
	
	if( 0 == rank ) {														// send to rank 1
		
		MPI_Send(buffer, NUM_BYTES, MPI_BYTE, 1, 0, MPI_COMM_WORLD);
		
	}else if(num_procs-1 == rank) {											// last rank doesn't send
		
		MPI_Recv(buffer, NUM_BYTES, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// debug
		//printf("rank %d received from %d\n", rank, rank-1);
		
	}else{
		
		// receive then send to next
		MPI_Recv(buffer, NUM_BYTES, MPI_BYTE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Send(buffer, NUM_BYTES, MPI_BYTE, rank+1, 0, MPI_COMM_WORLD);
		
		// debug
		//printf("rank %d received from %d\n", rank, rank-1);
		//printf("rank %d sending to %d \n", rank, 1);
		
	}
	
}


if( strcmp(bcast_implementation_name, "naive_bcast") == 0 ) {

	if( 0 == rank ) {
		
		// send to eveyone
		//printf("process %d sending to all\n", rank);
		for (j = 1; j < num_procs; j++) {
			MPI_Send(buffer, NUM_BYTES, MPI_BYTE, j, 0, MPI_COMM_WORLD);
		}
		
	}else{
		
		// receive
		MPI_Recv(buffer, NUM_BYTES, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// debug
		//printf("process %d received\n", rank);
		
	}
	
}
	







