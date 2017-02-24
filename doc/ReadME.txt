/*
===========================================================================================================================
Name: Naishil Shah
cruzid: nanshah@ucsc.edu 
===========================================================================================================================


There are 3 directories(folders) in the submission. Following is the explanation of the file organization:

src - This contains the source files of server and client, named servcheck.c and clicheck.c respectively. Also contains makefile.

bin - The direcotry contains the object and excecutables of the codes.

doc - Contains the README.txt file and Documentation.pdf file

---------------------------------------------------------------------------------------------------------------------------

***Steps to run: 

Assuming you are in the submitted folder.

1. cd src/

2. make myserver

3. make myclient

4. cd ../bin/
Run as many servers as required - one in each terminal by running the following command:

5. ./myserver <portnum> 
PLEASE NOTE: The portnumber entered should be in accordance with the number mentioned in the server-info.txt file

Run a different terminal for the client using the following command:

6. ./myclient server-info.txt <number of servers to be connected>


***Description: 

1. The code of the previous assignment was used to build this code. The servers work perfectly and need a port number in 
   their command. The only difference is that now the protocol used for the application in UDP.
2. Since this is a unreliable protocol, I have implemented the Stop and Wait protocol in the application, i.e. the server
   and the client wait after each transmisson until they recieve a correct acknowledgement. This ensures that the
   reliability of the contents in the packets are taken care of.
3. This functionality is achieved via the use of timer variables which wait for a desired amount of time before they send 
   back the original request.   
4. The client thread when used with the correct syntax as shown below, it is able to connect to the server and and check 
   whether the required file is present or not.
5. If the file is found, the client gets the file size from the first server and then creates threads to fetch the
   separate chunks from different servers.
6. The chunk size is calculated by dividing the size of the file by the number of servers provided by the user. If the 
   entered number is greater than the available servers, then all the servers will be used and the user will be notified.
7. The threads give user the information of the amount of chunk size and the offset they have received.  


***ISSUES and PROBLEMS NOT TACKLED***

1. Since this code is built on the second assignment, the previous errors are still present. As discussed with professor,
   I hope the grading is done accordingly and not many points are lost due to previous errors.
2. The improvement I was able to achieve was instead of the misplaced characters at the change of each thread, now there 
   are a few extra characters just at the end of the file.
3. Hence the TA is requested to check the output file by running it independently as the script will always produce an
   error. The steps to be followed are mentioned above.
   

Thank you for grading the assignment.

