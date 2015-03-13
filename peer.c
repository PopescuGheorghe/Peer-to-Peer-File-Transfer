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
#include "protocol.h"
//mod de rulare ./peer
//In momentul in care primeste de la serverul intermediar , un mesaj ->SEND_FILE

#define SERVER_PORT 5678
#define SERVER_IP	"127.0.0.1"
#define BUFFER_SIZE         100


int main(int argc , char *argv[]) {
	char pBuffer[BUFFER_SIZE];
	char buf[1024];
	int fd , sockfd ,connfd ;
	pid_t pidClient ;
	struct sockaddr_in local_addr ,remote_addr,foo;
	socklen_t len;
	char* fisier;
	fisier=(char *)malloc(20*sizeof(char));

	
	if (argc < 1)	{
		printf(" Numar incorect de argumente Ussage:fiser dorit, fisiere puse la dispozitie\n");
		exit(1);
	}

	if(argc >= 1 && atoi(argv[1]) == 1){
		//peer-ul doreste un fisier
		strcat(fisier,argv[1]);
		strcat(fisier," ");
		strcat(fisier,argv[2]);
		printf("Peer-ul doreste un fisier %s\n", fisier);
	}

	if(argc >= 1 && atoi(argv[1]) == 0) {
		//peer-ul a specificat ca nu doreste un fisier
		int i;
		for (i = 1; i < argc; ++i)
		{
			strcat(fisier,argv[i]);
			if(i < argc -1 )
				strcat(fisier," ");
		}
		printf("Peeru-l pune la dispozitie fisiere %s\n", fisier);
	}

	if( -1 == (sockfd=socket(PF_INET,SOCK_STREAM,0))){
		printf("Nu s-a putut crea soclul\n");
		exit(1);
	}
	
	set_addr(&local_addr,NULL,INADDR_ANY,0); //portul e alocat de sistemul de operare
	//Legare socket
	if(-1== bind(sockfd,(struct sockaddr *)&local_addr,sizeof(local_addr))){
		printf("Eroare la bind()\n");
		exit(1);
	}

	//Pana acum avem un soclu -> legat la local_addr si poate primi / trimite 
	//prin oricare din interfetele de retea , si un port alocat de sist de operare

	//Conectare la sever 
	set_addr(&remote_addr , SERVER_IP ,0 ,SERVER_PORT);
	if(-1 == connect(sockfd,(struct sockaddr *)&remote_addr,sizeof(remote_addr))){
		printf("Conectare la server esuata \n");
		exit(1);
	}	
	
		//sedning message


	printf("\nGot a connection");
    strcpy(pBuffer,fisier);
	printf("\nSending \"%s\" to server",pBuffer);
    /* number returned by read() and write() is the number of bytes
    ** read or written, with -1 being that an error occured
    ** write what we received back to the server */
    stream_write(sockfd,fisier,strlen(pBuffer)+1);
    /* read from socket into buffer */
    /*read(sockfd,pBuffer,BUFFER_SIZE);*/
	printf("\nClosing the socket");
    /* close socket */
    if(close(sockfd) == -1)
        {
         printf("\nCould not close socket\n");
         return 0;
        }


	
	
}
