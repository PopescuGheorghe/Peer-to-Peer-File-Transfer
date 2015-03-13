#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "protocol.h"

#define SERVER_PORT 5678
#define MAX_FILES 10
#define MAX_PEERS 20
#define THREADS_NO 20
#define BUFFER_SIZE         100


 struct Info {
	char 	  peer_ip[16];
	int	  	  peer_port;
	char	  files[MAX_FILES];
};




/*void*( receive_files)(void *arg){

	struct Info inf[MAX_PEERS] ;
	int pos=0 ;

	//int *sock = (int *)arg;
	struct sockaddr_in sock =*((struct sockaddr_in *)arg);
	//inf[pos]->files=(char *)malloc()
	 stream_read(sock,&inf[pos].files,1);
	 strcpy(inf[pos].peer_ip,inet_ntoa(sock.sin_addr));
	 inf[pos].peer_port = sock.sin_port;
	 printf("Peer Port %d\n",inf[pos].peer_port);
	 pos++;
	 printf(" Number of peers %d\n", pos );
	 return &inf[pos];
}
*/


int main(int argc , char *argv[]) {

	char buf[1024];
	char pBuffer[BUFFER_SIZE];
	unsigned nReadAmount;

	struct Info inf[MAX_PEERS] ;
	int pos=0 ;

	int fd , sockfd ,connfd ;
	pid_t pidClient ;
	struct sockaddr_in local_addr ,remote_addr;
	socklen_t rlen;
	pthread_t th[THREADS_NO];
	int thread;
	
	if(argc > 1 ){
		printf("Rulare incorecta \n");
		exit(1);
	}

	if( -1 == (sockfd=socket(PF_INET,SOCK_STREAM,0))){
		printf("Nu s-a putut crea soclul\n");
		exit(1);
	}

	set_addr(&local_addr,NULL,INADDR_ANY,SERVER_PORT); 
	if( -1 == bind(sockfd, (struct sockaddr *) &local_addr, sizeof(local_addr))){
		printf(" Eroare la bind \n");
		exit(1);
	}
	if(-1 == listen(sockfd, 10)) {
		printf("Eroare la listen \n");
		exit(1);
	}
	rlen = sizeof(remote_addr);

	while(1){
		connfd = accept(sockfd,(struct sockaddr*)&remote_addr, &rlen);
		if (connfd < 0)
		{
			printf("Eroare la accept \n");
			exit(1);
		}

		/*pthread_create(&th[0] , NULL , &receive_files  ,&connfd );*/
		/*stream_read(connfd,&inf[pos].files,1);*/
		/*printf("IP address is: %s\n",inet_ntoa(remote_addr.sin_addr));
    	printf("port is: %d\n", (int) ntohs(remote_addr.sin_port));
    	printf("Peer Port %d\n",ntohs(inf[pos].peer_port ));*/
    	/*inf[pos].peer_ip = remote_addr.sin_port;
    	printf("Peer port %d\n",inf[pos].peer_port);
    	pos++;*/




    	strcpy(inf[pos].peer_ip,inet_ntoa(remote_addr.sin_addr));
    	printf("Ip: %s\n", inf[pos].peer_ip);
    	inf[pos].peer_port = remote_addr.sin_port;
	 	printf("Peer Port %d\n",inf[pos].peer_port);

		
	

        //receving message

	    nReadAmount=stream_read(connfd,pBuffer,BUFFER_SIZE);
	    strcpy(inf[pos].files,pBuffer);
	    printf("\nReceived \"%s\" from client\n",pBuffer);
	    /* write what we received back to the server */
	    /*write(connfd,pBuffer,nReadAmount);
	    printf("\nWriting \"%s\" to server",pBuffer);*/
	    pos++;
	    printf("\nClosing socket\n");
	    /* close socket */                       
	    if(close(connfd) == -1)
	    {
	        printf("\nCould not close socket\n");
	        return 0;
	   	 }

	    }
	    

exit(0);
}