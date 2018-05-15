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
    int m, n;
    
    if( argc != 2)
    {
     cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
     return -1;
    }
    
    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
      cout <<  "Could not open or find the image" << std::endl ;
      return -1;
    }
    
    ////// Open CV lib call
    // Mat blur_opencv
    // blur(image, blur_opencv, Size(3,3));
    // imwrite("opencv_blue.jpg",blur_opencv);

    m = image.rows;
    n = image.cols;
    
    A = alloc2d(m+2 , n+2);
    B = alloc2d(m/2, n);
    C = alloc2d(m/2, n);
    
    // master
    if (myrank == 0) {
    
      MPI_Status status;
      // padding with 0
      for(int i = 0; i < m+2; ++i)
      {
        if(i ==0 || i == m+1)
        {
          for(int j =0; j < n+2; ++j)
          {
            A[i][j] = 0;
          }
        }
        else
        {
          A[i][0] = 0;
          A[i][n+1] = 0;
        }
      }
      // read image
      for (int i = 0; i < m; ++i)
      {
        for (int j = 0; j < n ; ++j)
        {
          A[i+1][j+1] = image.at<uchar>(i , j);
        }
      }
       

      MPI_Send(&(A[0][0]), (m+2)*(n+2), MPI_INT, 1, 1, MPI_COMM_WORLD);
      MPI_Send(&m, 1, MPI_INT, 1, 2, MPI_COMM_WORLD);
      MPI_Send(&n, 1, MPI_INT, 1, 3, MPI_COMM_WORLD);
      
      MPI_Send(&(A[0][0]), (m+2)*(n+2), MPI_INT, 2, 4, MPI_COMM_WORLD);
      MPI_Send(&m, 1, MPI_INT, 2, 5, MPI_COMM_WORLD);
      MPI_Send(&n, 1, MPI_INT, 2, 6, MPI_COMM_WORLD);
      
      MPI_Recv(&(B[0][0]), m*n/2, MPI_INT, 1, 7, MPI_COMM_WORLD, &status);
      MPI_Recv(&(C[0][0]), m*n/2, MPI_INT, 2, 8, MPI_COMM_WORLD, &status);

      for(int i = 0 ; i < m/2; i++)
      {
          for(int j = 0; j < n; j++)
          {
            A[i+1][j+1] = B[i][j];
          }
      }
      for(int i = 0 ; i < m/2; i++)
      {
        for(int j = 0; j < n; j++)
        {
          A[i+m/2+1][j+1] = C[i][j];
        }
      }
      Mat image2(n, m, CV_8U);
      for (int i = 0; i < n ; ++i)
      {
        for (int j = 0; j < m ; ++j)
        {
          image2.at<uchar>(i , j) = A[i+1][j+1];
        }
      }  
      imwrite("gray_blur_parallel.jpg",image2);
    }
    if (myrank == 1) 
    {
      MPI_Status status;
      MPI_Recv(&(A[0][0]), (m+2)*(n+2), MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
      MPI_Recv(&m, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &status);
      MPI_Recv(&n, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
      
      for(int i = 1 ; i <= m/2; i++)
      {
        for(int j = 1; j <= n; j++)
        {
          B[i-1][j-1] = A[i-1][j-1] + A[i-1][j+1] + A[i+1][j-1] + A[i+1][j+1];
          B[i-1][j-1] += 2*(A[i-1][j] + A[i][j+1] + A[i][j-1] + A[i+1][j]);
          B[i-1][j-1] += 4*(A[i][j]);
          B[i-1][j-1] /= 16;
        }
      }
      MPI_Send(&(B[0][0]), m*n/2, MPI_INT, 0, 7, MPI_COMM_WORLD);
      printf("Slave01 done calculation\n");
    }
    if (myrank == 2)
    {
      MPI_Status status;
      MPI_Recv(&(A[0][0]), (m+2)*(n+2), MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
      MPI_Recv(&m, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, &status);
      MPI_Recv(&n, 1, MPI_INT, 0, 6, MPI_COMM_WORLD, &status);

      for(int i = 1 ; i <= m/2; i++)
      {
        for(int j = 1; j <= n; j++)
        {
          /*
          1 2 1
          2 4 2
          1 2 1
          */
          C[i-1][j-1]    = A[i-1+ m/2][j-1] + A[i-1+ m/2][j+1] + A[i+1+ m/2][j-1] + A[i+1+ m/2][j+1];
          C[i-1][j-1] += 2*(A[i-1+ m/2][j] + A[i+ m/2][j+1] + A[i+ m/2][j-1] + A[i+1+ m/2][j]);
          C[i-1][j-1] += 4*(A[i+ m/2][j]);
          C[i-1][j-1] /= 16;
        }
      }
      MPI_Send(&(C[0][0]), m*n/2, MPI_INT, 0, 8, MPI_COMM_WORLD);
      printf("Slave02 done calculation\n");
    }
    
    MPI_Finalize();

    //waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}
