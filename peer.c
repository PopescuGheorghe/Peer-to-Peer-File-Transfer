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

#define SERVER_PORT 		5678
#define SERVER_IP			"127.0.0.1"
#define BUFFER_SIZE         100
#define PEER_PORT           5555
#define BUFFER_SIZE			100


int main(int argc , char *argv[]) {
	char pBuffer[BUFFER_SIZE],qBuffer[BUFFER_SIZE];
	char buf[1024];
	int fd , sockfd ,connfd,nread ;
	pid_t pidClient ;
	struct sockaddr_in local_addr ,remote_addr,rmt_addr;
	socklen_t rlen;
	char* fisier;
	int i,port;
	unsigned nReadAmount;
	fisier=(char *)malloc(20*sizeof(char));
	char ip_adress[16],temp_port[10],temp_file[10];

	//new peer server variables

	int new_fd, new_sockfd,new_connfd;
	struct sockaddr_in new_local_adress,new_remote_adress,new_rtm_adr;
	int nfis;
	socklen_t new_rlen;
	char file_name[10];

	if (argc < 1)	{
		printf(" Numar incorect de argumente Ussage: 0/1 Fisiere \n");
		exit(1);
	}

	if( -1 == (sockfd=socket(PF_INET,SOCK_STREAM,0))){
		printf("Nu s-a putut crea soclul\n");
		exit(1);
	}
	
	set_addr(&local_addr,NULL,INADDR_ANY,0); //portul e alocat de sistemul de operare
	//Legare socket
	if(-1== bind(sockfd,(struct sockaddr *)&local_addr,sizeof(local_addr))){
		printf("Eroare la bind() peer \n");
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



	switch(atoi(argv[1])){
		case 1 :
		//peer-ul doreste un fisier
		strcat(fisier,argv[1]);
		strcat(fisier," ");
		strcat(fisier,argv[2]);
		strcpy(temp_file,argv[2]);
		printf("Peer-ul doreste un fisier %s\n", fisier);
		char file_name[10];
		strcpy(pBuffer,fisier);
		stream_write(sockfd,fisier,BUFFER_SIZE);
		nReadAmount=stream_read(sockfd,qBuffer,BUFFER_SIZE);
		printf("%s\n",qBuffer );
		const char s[2] = " ";
		char *token;

	    /* get the first token */
		token = strtok(qBuffer, s);
		int i = 0;
	    /* walk through other tokens */
		while( token != NULL ) 
		{
			if(i == 0)
				strcpy(ip_adress,token);
			else if(i == 1)
				strcpy(temp_port,token);
			token = strtok(NULL, s);
			i++;
		}
		printf("IP adress is:%s\n",ip_adress );
		port = atoi(temp_port);
		printf( "Port is: %d\n",port );

		printf("Closing the socket \n");
   			 /* close socket */

		if(close(sockfd) == -1)
		{
			printf("\nCould not close socket\n");
			return 0;
		}

   		//mod client

		new_sockfd = socket(PF_INET,SOCK_STREAM, 0);
		set_addr(&new_local_adress, NULL, INADDR_ANY, 0);
		bind(new_sockfd, (struct sockaddr *) &new_local_adress,sizeof(new_local_adress));
		set_addr(&new_remote_adress, SERVER_IP, 0,PEER_PORT);
		printf("Set the address\n");
		connect(new_sockfd, (struct sockaddr*)&new_remote_adress,sizeof(new_remote_adress));

		printf("Mod Client: Receptie pregatita \n");
    	/* scrie un fisier nou */
		snprintf(file_name, 10, "fisier%.3d",1);

		if((new_fd = open(file_name,O_WRONLY|O_CREAT|O_TRUNC,00644)) < 0)
		{
			printf("Error creating destination file\n");
			exit(3);
		}

		printf("Mod Client: Fisier creat, astpet sa scriu date\n");
		if(new_fd == -1) {
			printf("Nu pot scrie fisierul %s\n",
				file_name);
			exit(1);
		}
		//while(0 < (nread = stream_read(new_sockfd,(void *)buf,1024))) {
			//write(new_fd, (void *)buf, BUFFER_SIZE);
	//}
		while(0 < (nread = read(new_sockfd,(void *)buf,1024))) {
			printf("Am primit %s\n",buf );
			stream_write(new_fd, (void *)buf, nread);
		}
		if(nread < 0)
			printf("Eroare la citirea de la retea\n");
		else
			printf("%s receptionat\n",file_name);
		close(new_fd);
		close(new_connfd);
		
	
	close(new_sockfd);


	break;


	case 0 : 
		//peer-ul a specificat ca nu doreste un fisier

	for (i = 1; i < argc; ++i)
	{
		strcat(fisier,argv[i]);
		if(i < argc -1 )
			strcat(fisier," ");
	}
	printf("Peeru-ul pune la dispozitie fisiere %s\n", fisier);

	strcpy(pBuffer,fisier);
	stream_write(sockfd,pBuffer,BUFFER_SIZE);

   		// modul server 

	//sleep(10000);

	fd = open(argv[2],O_RDONLY);
	printf("Fisier dorit %s\n", argv[2]);

	new_sockfd = socket(PF_INET,SOCK_STREAM, 0);
	set_addr(&new_local_adress, NULL, INADDR_ANY,
		PEER_PORT);
	bind(new_sockfd, (struct sockaddr *) &new_local_adress,
		sizeof(new_local_adress));

  /* trimite fisierul */
	listen(new_sockfd, 5);
	while(1) {
		printf("Mod Server: Initializare conexiune\n");
		new_rlen = sizeof(new_rtm_adr);
		printf("Waiting for connection...\n");
		if(-1 == (new_connfd = accept(new_sockfd, (struct sockaddr*)&new_rtm_adr,&new_rlen)))
		{
			printf("Eroare la accept\n");
			exit(1);
		}
		printf("Mod Server: Conexiune realizata\n");
			/*
			/* trimite fisierul */
		while(0 < (nread = read(fd,(void *)buf,BUFFER_SIZE))) {
			stream_write(new_connfd, (void *)buf, nread);
			printf("Am trimis:%s\n",buf);
		}
		if(nread < 0) {
			printf("Eroare la citirea din fisier\n");
			exit(1);
		}
		
		printf("Fisierul a fost trimis cu succes\n");
		
		close(fd);
		close(new_connfd);
	}
		close(new_sockfd);
		
		


	}
}
