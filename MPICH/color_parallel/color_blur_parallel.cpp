#include <mpi.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

/////////////////////////////////////
int ** alloc2d(int rows, int cols) {
    int * data = (int *)malloc(rows * cols * sizeof(int));
    int ** arr = (int **)malloc(rows * sizeof(int *));
    for(int i = 0; i < rows; ++i)
    {
      arr[i] = &(data[cols * i]);
    }
    return arr;
}

int main( int argc, char** argv )
{
    int nprocs, myrank;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
 
    int **A, **B, **C;
    int rows, cols, channel;
    
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
    
    Mat image, image2;
    // Read the file
    
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR); // color
    channel = 3;
    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }
    
    rows = image.rows;
    cols = image.cols;
    
    A = alloc2d(rows+2, cols+2);
    B = alloc2d(rows/2 , cols);
    C = alloc2d(rows/2 , cols);
    
    // master
    if (myrank == 0) {
      MPI_Status status;
      MPI_Send(&rows,    1, MPI_INT, 1, 0, MPI_COMM_WORLD);
      MPI_Send(&cols,    1, MPI_INT, 1, 1, MPI_COMM_WORLD);
      MPI_Send(&channel, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
      MPI_Send(&rows,    1, MPI_INT, 2, 3, MPI_COMM_WORLD);
      MPI_Send(&cols,    1, MPI_INT, 2, 4, MPI_COMM_WORLD);
      MPI_Send(&channel, 1, MPI_INT, 2, 5, MPI_COMM_WORLD);
      image2 = image.clone();
      // image initialize
      for(int i = 0; i < rows+2; ++i)
      {
        if(i ==0 || i == rows+1)
        {
          for(int j =0; j < cols+2; ++j)
          {
            A[i][j] = 0;
          }
        }
        else
        {
          A[i][0] = 0;
          A[i][cols+1] = 0;
        }
      }

      for(int cha = 0; cha < channel; cha++)
      {
        printf("Channel %d starts...\n", cha);
        for (int i = 0; i < rows; ++i)
        {
          for (int j = 0; j < cols ; ++j)
          {
            A[i+1][j+1] = image.at<cv::Vec3b>(i , j)[cha];
          }
        }    

        MPI_Send(&(A[0][0]), (rows+1)*(cols+1), MPI_INT, 1,   4*cha+6, MPI_COMM_WORLD);
        MPI_Send(&(A[0][0]), (rows+1)*(cols+1), MPI_INT, 2,   4*cha+7, MPI_COMM_WORLD);
        MPI_Recv(&(B[0][0]), rows*cols/2, MPI_INT, 1, 4*cha+8, MPI_COMM_WORLD, &status);
        MPI_Recv(&(C[0][0]), rows*cols/2, MPI_INT, 2, 4*cha+9, MPI_COMM_WORLD, &status);

        for(int i = 0 ; i < rows/2; i++)
        {
            for(int j = 0; j < cols; j++)
            {
              A[i+1][j+1] = B[i][j];
            }
        }   
        for(int i = 0 ; i < rows/2; i++)
        {
          for(int j = 0; j < cols; j++)
          {
            A[i+rows/2+1][j+1] = C[i][j];
          }
        }

        for (int i = 0; i < rows ; i++)
        {
          for (int j = 0; j < cols ; j++)
          {
            image2.at<cv::Vec3b>(i , j)[cha] = A[i+1][j+1];
          }
        }  
        printf("Channel %d ends...\n", cha);
      }
      printf("Done!\n");
      imwrite("color_blur_parallel.jpg",image2);
    }
    if (myrank == 1) 
    {
      MPI_Status status;
      MPI_Recv(&rows,    1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols,    1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&channel, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
      for(int cha = 0; cha < channel; ++cha)
      {
        MPI_Recv(&(A[0][0]), (rows+1)*(cols+1), MPI_INT, 0, 4*cha+6, MPI_COMM_WORLD, &status);
        printf("Slave01 received tag %d, processing...\n",4*cha+6);
        for(int i = 1 ; i <= rows/2; i++)
        {
          for(int j = 1; j <= cols; j++)
          {
            B[i-1][j-1] = A[i-1][j-1] + A[i-1][j+1] + A[i+1][j-1] + A[i+1][j+1];
            B[i-1][j-1] += 2*(A[i-1][j] + A[i][j+1] + A[i][j-1] + A[i+1][j]);
            B[i-1][j-1] += 4*(A[i][j]);
            B[i-1][j-1] /= 16;
          }
        }
        printf("Slave01 finished tag %d, reply by tag %d\n",4*cha+6, 4*cha+8);
        MPI_Send(&(B[0][0]), rows*cols/2, MPI_INT, 0, 4*cha+8, MPI_COMM_WORLD);
      }
      printf("Slave01 done calculation\n");
    }
    if (myrank == 2)
    {
      MPI_Status status;
      MPI_Recv(&rows,    1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
      MPI_Recv(&cols,    1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
      MPI_Recv(&channel, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
      for(int cha = 0; cha < channel; ++cha)
      {
        MPI_Recv(&(A[0][0]), (rows+1)*(cols+1), MPI_INT, 0, 4*cha+7, MPI_COMM_WORLD, &status);
        printf("Slave02 received tag %d, processing...\n",4*cha+7);
        for(int i = 1 ; i <= rows/2; i++)
        {
          for(int j = 1; j <= cols; j++)
          {
            C[i-1][j-1]    = A[i-1+ rows/2][j-1] + A[i-1+ rows/2][j+1] + A[i+1+ rows/2][j-1] + A[i+1+ rows/2][j+1];
            C[i-1][j-1] += 2*(A[i-1+ rows/2][j] + A[i+ rows/2][j+1] + A[i+ rows/2][j-1] + A[i+1+ rows/2][j]);
            C[i-1][j-1] += 4*(A[i+ rows/2][j]);
            C[i-1][j-1] /= 16;
          }
        }
        printf("Slave02 finished tag %d, reply by tag %d\n",4*cha+7, 4*cha+9);
        MPI_Send(&(C[0][0]), rows*cols/2, MPI_INT, 0, 4*cha+9, MPI_COMM_WORLD);
      }
      printf("Slave02 done calculation\n");
    }
    
    MPI_Finalize();

    //waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}