#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>

int sockfd, client_len, portnum, bytes, n, c, offset_size, chunk_size, err, size_check, bytes_read;
int file_size = 0;
int option = 1, file_avail = 1, exitconn = 1;
char send_buff[256], recev_buff[256], buffer[256], name[256];
struct sockaddr_in server_addr, client_addr;
int slen=sizeof( server_addr );
int clen=sizeof( client_addr );
struct stat s = {0};
char *strlocation;
fd_set readfd;
struct timeval tv;
FILE *file_pointer;
int maxfd;

void werr(int bytes)
{
	if( bytes == -1 )
     	printf(" Writing error.\n");
}

/*int estab_connection( int argc, char *argv[] )
{
	// listening for clients:
	printf("Waiting for client\n");
	listen( sockfd, 10 );
    client_len = clen;

    //Accepting connection from client 
    sockfd = accept( sockfd, (struct sockaddr *) &client_addr, &client_len );
    werr(sockfd);
    if ( sockfd < 0 ) exit(1);
	
	printf("Connected to Client\n");
    return sockfd;
}
*/

int check_request()
{
	printf("waiting for client\n");
	bzero( recev_buff, 256 );
	if( (n = recvfrom( sockfd, recev_buff, 256, 0, ( struct sockaddr * )&client_addr, &clen ) ) != 0)
    {
		puts( recev_buff );
		if(strstr( recev_buff, "CHECK" ) != NULL)
		{
        	option = 1;		//Setting option flag as 1 i.e. file size is requested
	    	bzero( recev_buff, 256 );    
            bzero( send_buff, 256 );
            strcpy( send_buff, "ACK" );
            n =  sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);
			werr( n );
		}
        else
        {
			if( strstr( recev_buff, "DATA" ) != NULL )
          	{
          		option = 2; //Here data is requested
             	bzero( recev_buff, 256 );                                      
            	bzero( send_buff, 256 );
            	strcpy( send_buff, "ACK" );
            	bytes =  sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);
            	werr ( bytes );
        	}
		}
  }
  else 
	printf( "Error receiving from client.\n" );
  return option;
}


int respond( int option )
{
	if( option == 1 )
    {
		if( ( n = recvfrom( sockfd, recev_buff, 256, 0, ( struct sockaddr * )&client_addr, &clen ) ) != 0 )
		{
            strlocation = strstr( recev_buff, "End of file" );
        	printf("\n The file name sent by Client is: %s", recev_buff );  

		}

        // Check whether file exists

        if( access( recev_buff, F_OK ) != -1 )
        	file_avail= 1;
        else 
        	file_avail = 0;

        // Obtain file size
        bzero( send_buff,256 );  
            
        if( file_avail == 1 )
        {      
        	stat( recev_buff, &s );
        	file_size = s.st_size;
        	sprintf( send_buff, "%d", file_size );
       }
       else
       		strcpy( send_buff, "NO FILE" );

            // Sending file size or NO FILE message
        n =  sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);
	werr( n );        
     }
            
	else if( option == 2 )
    {
		bzero( recev_buff,256 );  

          //Reading the file name
  		if( (n = recvfrom( sockfd, recev_buff, 256, 0, ( struct sockaddr * )&client_addr, &clen ) ) != 0 )
			strcpy( name,recev_buff );
			
		// Send ACK for name

		bzero( send_buff, 256 );
		strcpy( send_buff, "ACK" );
		n =  sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);
		werr( n );

          // Read Offset Size
		bzero( recev_buff, 256 );
        FD_ZERO( &readfd );
        FD_SET( sockfd, &readfd );
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        select ( maxfd, &readfd, NULL, NULL, &tv );

        if(FD_ISSET( sockfd, &readfd ))
        {
			n = recvfrom( sockfd, recev_buff, 256, 0, (struct sockaddr *) &client_addr, &clen );
            offset_size = atoi(recev_buff);
		}
          	
          // Send ACK for Offset_Size
  	bzero( send_buff, 256 );
    strcpy( send_buff, "ACK" );
    n =  sendto( sockfd, send_buff, n, 0, ( struct sockaddr * ) &client_addr, clen);
    werr( n );

      // Read chunk_size
	bzero( recev_buff, 256 ); 
   
	FD_ZERO( &readfd );
	FD_SET( sockfd, &readfd );
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	select ( maxfd, &readfd, NULL, NULL, &tv );
   
	if( FD_ISSET( sockfd, &readfd ) )
	{
		n = recvfrom( sockfd, recev_buff, 256, 0, (struct sockaddr *) &client_addr, &clen );
		chunk_size = atoi(recev_buff);
	}
        
    printf("\n The offset size received is: %d",offset_size);
    printf("\n The chunk size received is: %d",chunk_size);
        
            // Send the Data

    file_pointer = fopen( name, "r" );
	if ( file_pointer == NULL )
		{
            printf("\nERROR: Could not open File");
            return 1;   
    	}  

    n = fseek( file_pointer, offset_size, SEEK_SET );
    werr ( n );
    size_check = chunk_size/256;   // Calculating loop iterations
            
    bzero( send_buff,256 );

    n = 0;

    while (n != ( size_check+1 ) ) 
    {
		bytes_read = fread( send_buff, 1, 256, file_pointer );
		if( bytes_read > 0 )
            sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);

	
	    if ( bytes_read < 256 )
        {
	    	if ( feof( file_pointer ) )
            	printf("End of file\n");
        	if (ferror( file_pointer ) )
            	printf("ERROR: Could not read file. \n");
            break;
        }
        n++;
  }

   bzero( send_buff,256 );
   strcpy( send_buff,"ENDOFFILE" );
       
   n =  sendto( sockfd, send_buff, 256, 0, ( struct sockaddr * ) &client_addr, clen);
       werr ( n );

     }	
     
   return 0;
}

void main( int argc, char *argv[] )
{
    	
	if ( argc != 2 )  //Check for valid input
	{
      printf("usage %s port\n", argv[0]);
      exit(0);
   	}
   	
   		portnum = atoi( argv[1] ); 
		
		sockfd = socket( AF_INET, SOCK_DGRAM, 0 ); 

		if( sockfd == -1 )
		{
			printf( "Error creating socket" );
			exit(0);
		}
		
		maxfd = socket + 1;
		// Initialize socket


		//bzero( (char *) &server_addr, sizeof(server_addr) );
		memset(( char * ) &server_addr, 0, sizeof( server_addr ) ); 
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons( portnum );

		bytes = bind( sockfd, ( struct sockaddr * ) &server_addr, sizeof( server_addr ) );
		if ( bytes < 0 ) 
		{
			printf( "Error binding" );
			exit(0);
		}
   
   		while( 1 )
		{
			option = 0;
			//sockfd = estab_connection( argc, argv );
			option=check_request();
			respond( option );
		}
}

