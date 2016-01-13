/*
** listener.c -- a datagram sockets "server" demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#define MYPORT "42799"	// the port users will be connecting to

#define MAXBUFLEN 100

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)  //This get_in_addr function is a copy of Beej's guide book
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char * argv[])
{
 // **************************Get hostname and IP address************************** 
        char **pptr;
        char str4 [INET_ADDRSTRLEN]; //IP addr
        struct hostent *hip;
        char hostname [128];
        gethostname (hostname,sizeof (hostname));

//********************************end of get hostname*************************
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	int numbytes;
	struct sockaddr_storage their_addr;
	char buf[MAXBUFLEN];
	socklen_t addr_len;
	addr_len = sizeof their_addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(hostname, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror(" socket");
			continue;
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("bind");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, " failed to bind socket\n");
		return 2;
	}

	freeaddrinfo(servinfo);

//******************************Read the file**************************************************

     FILE *fp;
     char doc[42];
     char doc1[42];
     char *insur[3];
     char insurcmp[3][11];
     int m,n;
     
     insur[0] = (char *)malloc(sizeof(char) * 3 * 14);

     if ((fp=fopen("doc2.txt","r"))==NULL)
         {
			 printf("Cannot open the doc file.\n");
			 exit(1);
		 }
     
     fread(doc,42,1,fp);
     fread(doc1,42,1,fp);
     insur[0]=strtok(doc,"\n");
     insur[0][13]='\0';
     insur[1]=strtok(NULL,"\n");
     insur[1][13]='\0';     
     insur[2]=strtok(NULL,"\n");
     insur[2][13]='\0';

     for(m=0;m<=2;m++)
        {
			for(n=0;n<=9;n++)
			   {
				   insurcmp[m][n]=insur[m][n];
			   }
		     insurcmp[m][10]='\0';   
		}

//******************************End of reading the file**************************************************


//*****************************************Print the first MSG************************************
        if ((hip=gethostbyname(hostname))==NULL)
         {
         printf("cannot get the IP address of %s\n", hostname);
         return 0; 
         }
        pptr=hip->h_addr_list;
         inet_ntop(hip->h_addrtype, *pptr, str4, sizeof(str4));
         printf("Phase 3: Doctor 2 has a static UDP port %s and IP address %s .\n",MYPORT,str4);
//*******************************************End of first MSG**************************************

//**********************************Gain the insurance info****************************************	
while(1)
    {
        if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,
		(struct sockaddr *)&their_addr, &addr_len)) == -1) 
		{
		  perror("recvfrom");
		  exit(1);
	     }

       struct sockaddr_in *udpaddr=(struct sockaddr_in*)&their_addr;
       int pport;
       pport=ntohs(udpaddr->sin_port);

	   buf[numbytes] = '\0';
	   printf("Phase 3: Doctor2 receives the request from the patient with port number %d and the insurance plan %s .\n",pport,buf);
	for(m=0;m<=2;m++)
	   {
		   if ((strcmp(insurcmp[m],buf))==0)
		      {
				  n=m;
			  }
	   }

     char *price;
     price=strtok(insur[n]," ");
     price=strtok(NULL," ");
     price[2]='\0';
   
	printf("Phase 3: Doctor 2 has sent estimated price %s $ to patient with port number %d .\n", price, pport);
	
	char *sendmsg="doc2";
	sendto(sockfd,price,3,0, (struct sockaddr *)&their_addr, addr_len);
	
	sendto(sockfd,sendmsg,4,0, (struct sockaddr *)&their_addr, addr_len);
}	

	return 0;
}
