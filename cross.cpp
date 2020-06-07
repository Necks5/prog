#include <mpi.h>

#include <mkl.h>

#include <cmath>
#include <cstdio>
#include <cstdlib>

#define world MPI_COMM_WORLD

#define matrix_size 8192

double MatrixFormula(int i, int j)
{
	return 1.0 / (i + j + 1);
	//if (i == j) return 1.0; else return 0.0;
	return sin((double) (3.0 * i + 7.0 * j + 1.0));
};


int main(int argc, char ** argv)
{
	int mpi_world_size_sq;
	int mpi_size;
	int myid;
	int myid_I;
	int myid_J;

	int mtx_size;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(world, &mpi_world_size_sq);
	MPI_Comm_rank(world, &myid);

	mpi_size = sqrt(mpi_world_size_sq);
	mtx_size = matrix_size / mpi_size;
	if (mpi_size * mpi_size != mpi_world_size_sq || mtx_size * mpi_size != matrix_size)
	{
		if (myid == 0)
		{
			printf("The number of processes is not a full square or matrix size is not divisible by process count, exiting...");
		}; 

		MPI_Finalize();
		return -2;
	};

	myid_I = myid % mpi_size;
	myid_J = myid / mpi_size;

	char cN = 'N';
	double done = 1.0;
	double dzero = 0.0;
	double dmone = -1.0;
	int ione = 1;
	int izero = 0;

	double * mtx = new double[mtx_size * mtx_size];

	//filling my matrix parts
	int istart;
	int jstart;

	//matrix A
	istart = myid_I * mtx_size;
	jstart = myid_J * mtx_size;
	
	/*for(int i = 0; i < mpi_world_size_sq; i++) {
		MPI_Barrier(world);
		if(i == myid)
			printf("%d * %d %d \n", istart, jstart, mtx_size); 
	}*/

	for(int i = 0; i < mtx_size; i++)
	{
		for(int j = 0; j < mtx_size; j++)
		{
			mtx[i + j * mtx_size] = MatrixFormula(istart + i, jstart + j);
		};
	};
	
	bool flag = true;

	while(flag) {
	
		int nodeij[2];
		double nodemax = 0.0;
		int sign = 1;
		for(int i = 0; i < mtx_size; i++) {
			for(int j = 0; j < mtx_size; j++) {
				if(fabs(mtx[i + j * mtx_size]) > fabs(nodemax)) {
					nodemax = mtx[i + j * mtx_size];
					nodeij[0] = istart + i;
					nodeij[1] = jstart + j;
					if(mtx[i + j * mtx_size] < 0) 
						sign = -1;
				}
	
			}
		}
		int* signs = (int*)malloc(mpi_world_size_sq * sizeof(int));
		int* nodes_buf = (int*)malloc(mpi_world_size_sq * 2 * sizeof(int) );
		double* maxs_buf = (double*)malloc(mpi_world_size_sq * sizeof(double));
		MPI_Allgather(&nodeij[0], 2, MPI_INT, nodes_buf, 2, MPI_INT, world);
		MPI_Allgather(&nodemax, 1, MPI_DOUBLE, maxs_buf, 1, MPI_DOUBLE, world);
		MPI_Allgather(&sign, 1, MPI_INT, signs, 1, MPI_INT, world);

		int glob_i, glob_j;
		double glob_max = 0.0;
		int glob_sign = 1;
		for(int i = 0; i < mpi_world_size_sq; i++){
			if(fabs(maxs_buf[i]) > fabs(glob_max)) {
				glob_max = maxs_buf[i];
				glob_i = nodes_buf[2 * i];
				glob_j = nodes_buf[2 * i + 1];
				glob_sign = signs[i];
			}
		}

		//printf("%d %lf %d %d \n", myid, glob_max, glob_i, glob_j);
		
		double* ii = (double*)malloc(matrix_size * sizeof(double));
		double* jj = (double*)malloc(matrix_size * sizeof(double));
		int block_id_i = glob_i / mtx_size;
		int block_id_j = glob_j / mtx_size;

		for(int i = 0; i < matrix_size/mtx_size; i++) {
			//if(myid == 0)
			//	printf( "%d %d %d %d\n", myid, i, block_id_i, block_id_j);
			MPI_Barrier(world);
			int temp1=0;
			int *temps1 = (int*)malloc(mpi_world_size_sq * sizeof(int));
			if(block_id_i == myid_I && i == myid_J) {
				//printf("1if\n");
				for(int j = 0; j < mtx_size; j++)
					jj[j + myid_J*mtx_size] = mtx[glob_i%mtx_size + j * mtx_size];
				temp1 = myid;
			}
			MPI_Allgather(&temp1, 1, MPI_INT, temps1, 1, MPI_INT, world);
			int true_temp1 = -1;
			for(int as = 0; as < mpi_world_size_sq; as++) {
				if(temps1[as] > true_temp1) {
					true_temp1 = temps1[as];
				}
			}
			MPI_Barrier(world);
			//printf("1eeeee %d\n", true_temp1);
			MPI_Bcast(jj, matrix_size, MPI_DOUBLE, true_temp1, world);
			
			
			int temp2=0;
			int *temps2 = (int*)malloc(mpi_world_size_sq * sizeof(int));
			//MPI_Barrier(world);
			if(block_id_j == myid_J && i == myid_I) {
				//printf("2if\n");
				for(int j = 0; j < mtx_size; j++) 
					ii[j + myid_I*mtx_size] = mtx[j + (glob_j%mtx_size) * mtx_size];
				temp2 = myid;
			}
			MPI_Allgather(&temp2, 1, MPI_INT, temps2, 1, MPI_INT, world);
			int true_temp2 = -1;
			for(int as = 0; as < mpi_world_size_sq; as++) {
				if(temps2[as] > true_temp2) {
					true_temp2 = temps2[as];
				}
			}
			MPI_Barrier(world);
			MPI_Bcast(ii, matrix_size, MPI_DOUBLE, true_temp2, world);
			
		}
		for(int i = 0; i < mtx_size; i++) {
			for(int j = 0; j < mtx_size; j++) {
				mtx[i + j * mtx_size] -=  (ii[i + istart] * jj[j + jstart]) / glob_max;   
			}
		}
		/*
		for(int i = 0; i < mpi_world_size_sq; i++) {
			MPI_Barrier(world);
			if(i == myid) {
				for(int j = 0; j < matrix_size; j++) {
					printf("%d %lf %lf\n",j,  ii[j], jj[j]);
				}
			}
		}
		*/

		
		if(myid == 0)
			printf("max: %lf \n", fabs(glob_max));
		if(fabs(glob_max) < 0.001)
			flag = false;
		
		//flag = false;





	}



	delete[] mtx;
	
	MPI_Finalize();
};
