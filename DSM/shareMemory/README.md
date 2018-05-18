

First, we have to creat the shared memory with `write.cpp` and read the written data with `read.cpp`

    g++ write.cpp -o write
    g++ read.cpp -o read

and we can write the data into shared memory automatically created with

    ./write <data>

after wrote data into the shared memory, we may read `ONLY ONCE` with 

    ./read

Because in the read process, we detach and distroy after output the data in shared memory, therefore, release the memory


Additional:


List shared memory segments:

    ipcs -m

Remove shared memory segment created with shmkey:

    ipcrm -M key

Remove shared memory segment identified by shmid:

    ipcrm -m id