#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BSIZE 256
typedef struct sockaddr SA;

typedef struct thread_data_s{
    int thread_id;
    int offset_size;
    int chunk_size;
}thread_struct;


char IP_address[10][100];
char Port_Number[10][100];
struct sockaddr_in server_addr, client_addr, temp_addr;
int slen = sizeof( server_addr );
int clen = sizeof( client_addr );
int len = sizeof( temp_addr );
int offset_size[10];
int chunk_size[10];
char name[BSIZE];

void werr(int bytes)
{
	if( bytes == -1 )
    	printf("\n Writing error.");
}

int createAndConnect( int id, int pl )
{
	int new_socket = socket( AF_INET, SOCK_DGRAM, 0 );
    if( new_socket == -1 )
    	printf( "\nERROR:Could not create Socket\n" );
    	
    memset( ( char * ) &client_addr, 0, sizeof( client_addr ) ); 
    client_addr.sin_family      = AF_INET;
    client_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    client_addr.sin_port        = htons(0);
    
	// binding the Addresses with the Client Socket Descriptor
    bind( new_socket, (SA *) &client_addr, sizeof( client_addr ) );	

    bzero( &server_addr, sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( atoi(Port_Number[id]) );

    if( inet_pton(AF_INET, IP_address[id], &server_addr.sin_addr) <= 0 )
    {
    	printf( "\nINET_PTON error for IP: %s", IP_address[id] );
    	exit(0);
    }

    /*if( connect(new_socket, (SA *) &server_addr, sizeof(server_addr)) < 0 )
    {
    	printf( "Server %d not Available\n",id );
    	//return 10;
    	exit(0);
    }*/
    else
    {
    	if(pl==0) id--;
		printf( "\nClient Thread %d is Running. \n", id + 1 );
    }

	return new_socket;
}

void readfn( int client_socket, char *buf )
{
    int bytes = sendto( client_socket, buf, BSIZE, 0, ( struct sockaddr * )&server_addr, slen);
    werr( bytes );
    while( ( bytes = recvfrom( client_socket, buf, BSIZE, 0, ( struct sockaddr *)&server_addr, &slen ) ) != 0 )
    {
        if( strstr(buf, "ACK") != NULL )
            break;
    }
}

void *client_thread(void *struct_vals)
{
	char send_buff[BSIZE], recev_buff[BSIZE], tok[BSIZE];
	int id, err, bytes, client_socket;
  	FILE *output_file;
  	off_t position;
  	fd_set readfd;
	struct timeval tv;
	int maxfd;
	struct sockaddr_in server_addrt, client_addrt, temp_addrt;
	int slent= sizeof( server_addrt );
	int clent = sizeof( client_addrt );
	int lent= sizeof( temp_addrt );

	thread_struct *thread_data;
  	thread_data = (thread_struct *)struct_vals;

  	id = thread_data -> thread_id;
    printf( "\n\nId: %d", id + 1 );
    printf( "\nOffset Size: %d", thread_data -> offset_size );
    printf( "\nChunk Size: %d", thread_data -> chunk_size );

	client_socket = socket( AF_INET, SOCK_DGRAM, 0 );
    
    if( client_socket == -1 )
    	printf( "\nERROR:Could not create Socket\n" );
    	
    memset( ( char * ) &client_addrt, 0, sizeof( client_addrt ) ); 
    client_addrt.sin_family      = AF_INET;
    client_addrt.sin_addr.s_addr = htonl( INADDR_ANY );
    client_addrt.sin_port        = htons(0);
    
	// binding the Addresses with the Client Socket Descriptor
    bind( client_socket, (SA *) &client_addrt, sizeof( client_addrt ) );	

    bzero( &server_addrt, sizeof( server_addrt ) );
    server_addrt.sin_family = AF_INET;
    server_addrt.sin_port = htons( atoi(Port_Number[id]) );

    if( inet_pton( AF_INET, IP_address[id], &server_addrt.sin_addr ) <= 0 )
    {
    	printf( "\nINET_PTON error for IP: %s", IP_address[id] );
    	exit(0);
    }
	maxfd = client_socket + 1;
	
	bzero( tok, BSIZE );
    strcpy( tok, "DATA" );
	bytes = sendto( client_socket, tok, BSIZE, 0, ( struct sockaddr * )&server_addrt, slent );
    werr( bytes );
   
   
	bzero( recev_buff, BSIZE );
    
	FD_ZERO( &readfd );
    FD_SET( client_socket, &readfd );
	tv.tv_sec = 1;
    tv.tv_usec = 0;

    select ( maxfd, &readfd, NULL, NULL, &tv );

    if( FD_ISSET( client_socket, &readfd ) ) 
		bytes = recvfrom( client_socket, recev_buff, BSIZE, 0, ( struct sockaddr * ) &temp_addrt, &lent);
	
	
	//SENDING FILE NAME TO SERVER
    
    bzero( send_buff, BSIZE );
    strcpy( send_buff, name );
	
	bytes = sendto( client_socket, send_buff, BSIZE, 0, ( struct sockaddr * )&server_addrt, slent);
    werr( bytes );
   
	bzero( recev_buff, BSIZE );
    
	FD_ZERO( &readfd );
    FD_SET( client_socket, &readfd );
	tv.tv_sec = 1;
    tv.tv_usec = 0;

    select ( maxfd, &readfd, NULL, NULL, &tv );

    if( FD_ISSET( client_socket, &readfd ) ) 
		bytes = recvfrom( client_socket, recev_buff, BSIZE, 0, ( struct sockaddr * ) &temp_addrt, &lent);
	
	//SENDING OFFSET SIZE

    bzero( send_buff, BSIZE );
    sprintf( send_buff, "%d", thread_data -> offset_size );
    
    bytes = sendto( client_socket, send_buff, BSIZE, 0, ( struct sockaddr * )&server_addrt, slent);
    werr( bytes );
   
   
	bzero( recev_buff, BSIZE );
    
	FD_ZERO( &readfd );
    FD_SET( client_socket, &readfd );
	tv.tv_sec = 1;
    tv.tv_usec = 0;

    select ( maxfd, &readfd, NULL, NULL, &tv );

    if( FD_ISSET( client_socket, &readfd ) ) 
		bytes = recvfrom( client_socket, recev_buff, BSIZE, 0, ( struct sockaddr * ) &temp_addrt, &lent);
	
    //SENDING CHUNK SIZE
    
    bzero( send_buff, BSIZE );
    sprintf( send_buff, "%d", thread_data -> chunk_size );
    bytes = sendto( client_socket, send_buff, BSIZE, 0, ( struct sockaddr * )&server_addrt, slent);
    werr( bytes );
    
    bzero( recev_buff, BSIZE );

    output_file = fopen( "Output_File.txt", "w+" );
    
    if( output_file == NULL )
        printf("\nERROR: could not open file");
       
    err = fseek( output_file, thread_data -> offset_size, SEEK_SET );
    if( err < 0 )
        printf("\nERROR:could not traverse the file");

    FD_ZERO( &readfd );
    FD_SET( client_socket, &readfd );
	tv.tv_sec = 1;
    tv.tv_usec = 0;

    select ( maxfd, &readfd, NULL, NULL, &tv );

	if( FD_ISSET( client_socket, &readfd ) )
	{
		while (1)
		{
			bytes = recvfrom( client_socket, recev_buff, BSIZE, 0, (struct sockaddr *)&temp_addrt, &lent);
            if( strstr( recev_buff, "ENDOFFILE") != NULL)
				break;
               
            fwrite( recev_buff, 1, bytes, output_file);
		}
    }
    
    bzero( recev_buff, BSIZE );
    printf("\nFile Written by thread %d", id + 1 );

    fclose( output_file );
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("\nThe server-input file or the number of Servers is missing.");
		exit(0);
	}

	int	client_socket1, max = 0, n, bytes, count = 0, ch = 1, file_size, extra_chunk, i, th_ret[10];
	char recev_buff[BSIZE], tok[BSIZE], send_buff[BSIZE];
 	char *token;
 	pthread_t threads[10];
 	thread_struct thread_array[10];
  	FILE *input_file;
  	fd_set readfd;
	struct timeval tv;
	int maxfd;

    int number_of_servers = atoi(argv[2]);

    char const* const filename = argv[1];
    input_file = fopen(filename,"r");


    while( fgets( recev_buff, sizeof(recev_buff), input_file ) )
    {
		token = strtok( recev_buff, " " );
        while( token != NULL ) 
        {
        	switch(ch)
        	{
        		case 1:
           			strcpy( IP_address[max], token );
           			token = strtok( NULL, " " );
           			ch = 2;
           		break;
           
           		case 2:
               		strcpy( Port_Number[max], token );
            		token = strtok( NULL, " " );
            		ch = 1;
            	break;
           	}
       	}
    	max += 1;
    }

    fclose(input_file);

    if( number_of_servers > max )
    	printf("\nMax Servers: %d", max);
    
    count = (number_of_servers >= max) ? max : number_of_servers;

    client_socket1 = createAndConnect( 0, 0 );

    // SEND CHECK TOKEN TO SERVER
    
    bzero( tok, BSIZE );
    strcpy( tok, "CHECK" );
	
	
	bytes = sendto( client_socket1, tok, BSIZE, 0, ( struct sockaddr * )&server_addr, slen);
    werr( bytes );
   
	bzero( recev_buff, BSIZE );
    
	FD_ZERO( &readfd );
    FD_SET( client_socket1, &readfd );
	tv.tv_sec = 1;
    tv.tv_usec = 0;

    select ( maxfd, &readfd, NULL, NULL, &tv );

    if( FD_ISSET( client_socket1, &readfd ) ) 
		n = recvfrom( client_socket1, recev_buff, BSIZE, 0, ( struct sockaddr * ) &client_addr, &slen);
	
	
	//SEND THE FILE NAME
	
    
    printf("\nEnter file name: ");
    bzero( send_buff, BSIZE );
    fgets( send_buff, sizeof(send_buff), stdin );
    send_buff[ strcspn( send_buff,"\n" ) ] = '\0';

    strcpy( name, send_buff );
    bytes = sendto( client_socket1, name, BSIZE, 0, ( struct sockaddr * )&server_addr, slen);
	werr( bytes );
    bzero( send_buff, BSIZE );

    if( FD_ISSET( client_socket1, &readfd ) )
		n = recvfrom(client_socket1, recev_buff, BSIZE, 0, (struct sockaddr *) &client_addr, &slen);
    
    if( strstr( recev_buff, "NO FILE" ) != NULL )
	{
		printf("\nFile does not exist!!");
        exit(0);
	}
    else
    {
		file_size = atoi( recev_buff );
        printf("\nFile size: %d", file_size);
	}
    
    printf("\nNumber of servers: %d", count);
    offset_size[0] = 0;
    for( i = 0; i < count; i++ )
    	chunk_size[i] += file_size / count;

    extra_chunk = file_size % count;
    chunk_size[i-1] += extra_chunk;

    for( i = 1; i < count; i++ )
    	offset_size[i] = offset_size[i-1] + chunk_size[i-1];

    for( i = 0; i < count; i++ )
    {
      	thread_array[i].thread_id = i;
      	thread_array[i].offset_size = offset_size[i];
      	thread_array[i].chunk_size = chunk_size[i];
    } 

    for( i = 0; i < count; i++ )
    {
      	th_ret[i] = pthread_create( &threads[i], NULL, client_thread, (void *) &thread_array[i] );
      	if( th_ret[i] )
          	printf("\nERROR: could not create thread: %d", i + 1);
    }
    for( i = 0; i < count; i++ )
    {
      	if( pthread_join( threads[i], NULL ) != 0 )
	       printf("\nERROR: could not join thread: %d", i + 1);
    }
    printf("\nTeminating");
	return 0;
}




