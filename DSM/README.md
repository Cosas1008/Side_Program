This is the Distributed Shared Memory Project of simulating the multi-server ATM to single client with Redis as database

First, we have to compile the server and client with header and redis API lib `hiredis`

compile with  
	make 

Next, we can run the server with default port number 2015 to 2017 with example run   
	make runserver

Testfile name is set to sample.txt and the output result if sample_output.txt

	make testclient

Finally, clean the project with

	make clean


Upgrade 2: One-to-many client-server with non-blocking Req/Res

![rectangle](https://github.com/yuweichen1008/Side_Program/blob/master/DSM/src/Async_server.emf)  


NCTU CS