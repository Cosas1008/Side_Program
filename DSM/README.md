This is the Distributed Shared Memory Project of simulating the multi-server ATM to single client with Redis as database

First, we have to compile the server and client with header and redis API lib `hiredis`

compile with  

	make 

Which will compile with extend lib `c++11` and link to hiredis. Next, we can run the server with default port number 2015 to 2017 with example run   

	make runserver

Testfile name is set to sample.txt and the output result if sample_output.txt

	make testclient

To stop the server and release the TCP port

	make killserver

Finally, clean the project with

	make clean


This submission is the One-to-mang with non-blocking Req/Res


Upgrade 2: One-to-many client-server with non-blocking Req/Res

![Asymic](https://github.com/yuweichen1008/Side_Program/blob/master/DSM/src/asymc.jpg)  


NCTU CS