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
#include <string.h>

#define SERVER_PORT 		5678
#define PEER_PORT           5555
#define MAX_FILES 			100
#define MAX_PEERS 			20
#define THREADS_NO 			20
#define BUFFER_SIZE         100


struct Info {
	char 	  peer_ip[16];
	int	  	  peer_port;
	char	  available_files[MAX_FILES];
	char	  wanted_file[MAX_FILES];
	char	  all_files[MAX_FILES];
};


int main(int argc , char *argv[]) {

	char buf[1024];
	char pBuffer[BUFFER_SIZE];
	unsigned nReadAmount;
	char* a;
	a = (char *)malloc(20*sizeof(char));
	char* info_to_send;
	info_to_send = (char *)malloc(20*sizeof(char));
	char* files;
	files = (char *)malloc(20*sizeof(char));

	struct Info inf[MAX_PEERS] ;
	int pos=0 ;
	int fd , sockfd ,connfd ;
	pid_t pidClient ;
	struct sockaddr_in local_addr ,remote_addr;
	socklen_t rlen;
	
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
		
        //printg the ip and the port
		strcpy(inf[pos].peer_ip,inet_ntoa(remote_addr.sin_addr));
		printf("Ip: %s\n", inf[pos].peer_ip);
		inf[pos].peer_port = remote_addr.sin_port;
		printf("Peer Port %d\n",inf[pos].peer_port);
		printf("Peer sock %d\n",connfd);
		

        //receving message
		nReadAmount=stream_read(connfd,pBuffer,BUFFER_SIZE);
		strcpy(inf[pos].all_files,pBuffer);
		printf("\nReceived \"%s\" from client\n",inf[pos].all_files);

	    //check if the client wants files or uploads files
		if(inf[pos].all_files[0] =='0'){
			printf("Clientul pune la dispozitie fisiere\n");
			strcpy(inf[pos].available_files,(inf[pos].all_files+strlen(inf[pos].all_files)-(strlen(inf[pos].all_files)-2)));
			printf("Availabe files %s\n",inf[pos].available_files );
			printf("Nr de elemente in structura: %d\n", pos);
		}
		else if(inf[pos].all_files[0] == '1'){
			printf("Clientul doreste un fisier \n");
			printf("Nr de elemente in structura: %d\n", pos);
	    	//dividing into words
			int current = pos;
			strcpy(inf[pos].wanted_file,(inf[pos].all_files+strlen(inf[pos].all_files)-(strlen(inf[pos].all_files)-2)));    
			const char s[2] = " ";
			char *token;
			int i = 0;

			for(i = 0; i < pos; i++){
				strcpy(files,inf[i].available_files);
			    /* get the first token */
				token = strtok(files, s);
			    /* walk through other tokens */
				while( token != NULL ) 
				{
					if(strcmp(inf[current].wanted_file,token)==0){ 
						printf( "Fisier disponibil: %s\n",token );
						printf("Fisier dorit: %s\n",inf[current].wanted_file );
						printf("Fisier corespunde\n");
						strcpy(info_to_send,inf[i].peer_ip);
						strcat(info_to_send," ");
						sprintf(a,"%d",inf[i].peer_port);
						strcat(info_to_send,a);
						// send to peer mod 1
						stream_write(connfd,info_to_send,BUFFER_SIZE);
						i=pos; //avoid new "for loop"
						token=NULL;
						printf("%s\n",info_to_send);
						printf("Informatia a fost trmisa\n");
						
						
					}
					else {
						printf( "Fisier disponibil: %s\n",token );
						printf("Fisier dorit: %s\n",inf[current].wanted_file );
						printf("Fisier nu corespunde\n");
						token = strtok(NULL, s);
					}
					
				} 

			}

		}

		pos++;
		
	    /* write what we received back to the server */
	    /*write(connfd,pBuffer,nReadAmount);
	    printf("\nWriting \"%s\" to server",pBuffer);*/
		printf("\nClosing socket\n");
	    /* close socket */                       
		if(close(connfd) == -1)
		{
			printf("\nCould not close socket\n");
			return 0;
		}
	}
	close(sockfd);

	exit(0);
}
