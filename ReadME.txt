
README FILE FOR ASSIGNMENT 3

CE 156 

FOR: AVIRUDH KAUSHIK


***The List of files included in the Submission: 

The ex3.tar.gz file. 

When extracted there will be a ex3 folder. 

In the ex3 folder there will be 3 folders, 1 make file. 

This make file will be used for compiling the source code for the client and server applications.

In the DOC folder you will find a pdf file. The pdf contains the documentation report for the assignement.  

The source files will be found in the SRC folder. It contains 4 files. 

1: myclient.c - This is the file in which the client side of the application is written. 
2: myserver.c - This is the file in which the server side of the application is written. 
3: myunp.h
4: myunp.c 

Files 3 and 4 is a header file given to us from the TA which uses all the basic header files required by our application. 

When we run make in the ex3 directory the output files will be generated in the Bin directory. 

In the BIN directory you will find all the output files for the compiled client and server code along with the text files used for testing the code. There are two files Red.txt and Pigs.txt. 



***Description: 

The code for this assignment was built using the existing code from the previous assignment. 

The basic change which was involved in this assignment was to shift from TCP to UDP protocol for our message transfers. The server and the client code were modified to implement the UDP protocol.

On the client side the there 2 command line input which we have to give. 1st the server-info.txt file which contains the IP addresses and the port numbers of the servers which are available and the number of servers we wish to use. If the user requests for more servers than available then the maximum number of available servers are used. 

The client is able to connect to the server and get the size of the file requested by the client and if the file does not exist then it says the file does not exist. 

After getting the file size the client is able to create threads and ask them to fetch separate chunks from different servers. The client calculates the number of bytes to be divided among the servers based on the number of servers provided by the user. If the user inputs a number greater than the max number of servers then the client tells the user that it is going to use the max number of servers available instead. 

The threads run and connect to separate servers and fetch the chunk they are supposed to get and write on the output file.

The threads output the amount of chunk size and the offset they have received.  

The threads are able to get the separate chunks from different servers and write the data on the output file. 

The functionality of my program is as per the requirements of the Assignment i.e. to utilize UDP datagrams to transfer messages and files.  


***How to run: 

Run the make command in the ex3 directory. 

Go to the bin directory. 

Independently run the servers and client. 

Command for client: 

./myclient <server-info.txt> <number of servers you want to use>

Command for server: 

./myserver <port number>



***ISSUES and PROBLEMS NOT TACKLED***

One major issue in my program which does not satisfy the requirement of the assignment is that when the text file is copied from the server there are always a few characters which are missing in the end of the output file. This should be due to the buffer size I am using. 
The TA is requested to actually check the output file because I am sure the 'diff' command will always give some errors. I know I showed this error to the TA during a lab but I still was not able to properly nullify the problem. I tried some HACK i.e. to delete the extra characters from the end of the file so that the output is proper. But each file has a different offset to be deleted which made things more complicated. 
I would like to request the TA to check the actual output of the file. 

One more major problem in this assignment is that at times the threads write the data properly to the file but at times the threads don't write properly. Say there are 3 threads writing to the file each having a different offset. At times the whole file is written properly by threads. But at times the second chunk of data which is to be written to the file is filled with all NULL characters. Sometime only the first chunk is written to the file and sometimes only the last chunk is written to the file. SUPRISINGLY ! I tried to use mutexes which I thought could solve my problems but even then I was running into this problem. Ironically, my program runs better without mutexes than with mutexes. 

For proper performance, it is requested of the TA to delete the Output.text file after every time the client side of the program is run. This will ensure that the output generated is comparable to the original text file. My guess to that problem is the out of order delivery of the packets in UDP. I have not accounted for out of order delivery and retransmissions in my program but the program does run on UDP protocol and utilizes threads to write the data to the file. 

The error checking could not be done in detail given the time constraints of the assignment (AGAIN HAHA)
If one of the server is not available then none of the servers transfer files.Even the robustness of the servers is not taken into account for this assignment.If client crashes then the servers also crash. If the server gets a random IP number then it will give an error and stop the program. Also the retransmission could not be added. Concurrency of the servers is also not supported. My program is basically a modification of the previous submission into the UDP format.  



