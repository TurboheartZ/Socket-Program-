/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/unistd.h>//for gerhostname()***********************************
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT "21799" // the port client will be connecting to
#define MAXDATASIZE 100 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)  //This get_in_addr function is a copy of Beej's guide book
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
        // ********************************Read the file*********************************
        FILE *fp;
        char content[20],psw[4];
        char *name;
        char *pswl;
        int i;
        if((fp=fopen("patient1.txt","r"))==NULL) // Open the file and exit if fail
          {
            printf("Cannot open file\n");
            exit(1);
          }
        fgets(content,21,fp);
        name=strtok(content," ");
        pswl=strtok(NULL," ");
        for (i=0;i<=2;i++)
          {
             psw[i]=pswl[8+i];
          }
       psw[3]='\0';

        // ********************************End of read file*********************************
	


        // **************************Get hostname and IP address************************** 
        char **pptr;
        char str4 [INET_ADDRSTRLEN];
        char str6 [INET6_ADDRSTRLEN];
        int flag=1;
        struct hostent *hip;
        char hostname [128];
        gethostname (hostname,sizeof (hostname));
        if ((hip=gethostbyname(hostname))==NULL)
         {
         printf("cannot get the IP address of %s\n", hostname);
         return 0; 
         }
        pptr=hip->h_addr_list;
       
         switch(hip->h_addrtype)
        {
         case AF_INET: 
         inet_ntop(hip->h_addrtype, *pptr, str4, sizeof(str4));   
         break;
         case AF_INET6:
         inet_ntop(hip->h_addrtype, *pptr, str6, sizeof(str6));
         flag=0;    
         break;
         default:
         printf("cannot get the IP address of %s\n", hostname);
         break;
        }
        //*******************************END of getting IP addr****************************
        

    int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(hostname, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

        //***************************Get Port Number*********************************
        struct sockaddr_in sockpn;
        socklen_t len = sizeof(sockpn);
        getsockname(sockfd,(struct sockaddr *)&sockpn,&len);
        int ppn = ntohs(sockpn.sin_port);
        
        //***************************Get Port Number*********************************

        
        // ****************Print the first msg on screen***************************** 
        if (flag)              //flag is used to indicate if the addr is IPV4 or IPV6
        {
          printf("Phase 1: Patient 1 has TCP port number %d and IP address %s.\n", ppn,str4);
        }
        else
        {
          printf("Phase 1: Patient 1 has TCP port number %d and IP address %s.\n", ppn,str6);
        }

        //********************End of the first msg***********************************

        //****************Send and print the second msg******************************
        char auth[34]="authenticate ";        
        strcat(auth,name);
        strcat(auth," ");
        strcat(auth,pswl);
        send(sockfd,auth,strlen(auth)+1,0);
        printf("Phase 1: Authentication request from Patient1 with username %s and password %s has been sent to the Health Center Server. \n",name,psw);

        //********************End of the second msg***********************************


        freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';
	printf("Phase 1: Patient1 authentication result: %s\n",buf);
        //**************************Failure Check*******************************************
        int j;
        char fcheck[]="success\0";
        j=strcmp(fcheck,buf);
        if (j!=0)
           {
            close(sockfd);
           }
        printf("Phase 1: End of Phase 1 for Patient1.\n");
    //**************************End of Failure Check*************************************
	
		
	//****************************Send the available Msg**********************************
			send(sockfd,"available\0",10,0);
   //****************************End of  the available Msg**********************************
	//****************************RECV the available Msg**********************************
		char recav[72];
		char avcheck[13];

	 if ((recv(sockfd, recav, MAXDATASIZE-1, 0)) == -1) 
	    {
	       perror("recvavailability");
	       exit(1);
	    }
	 printf("Phase 2: The following appointments are available for Patient 1: \n%sPlease enter the preferred appointment index and press enter:\n",recav);
	 char indexnum;
     scanf("%c",&indexnum);
     getchar();
     while((indexnum<'1')||(indexnum>'6'))
	     {
			 printf("Please re-enter a correct time index.");
		     scanf("%c",&indexnum);
		     getchar();
		 }
	  char indexsel[12]="selection  ";
	  indexsel[10]=indexnum;
	  indexsel[11]='\0';
	  send(sockfd,indexsel,11,0); 
	  
	  while((recv(sockfd,avcheck,12,0))<5)
	     {
		 }
		 avcheck[12]='\0';
	  if((strcmp(avcheck,"notavailable\0"))==0)
	     {
			 close(sockfd);
			 printf("Phase 2: The requested appointment from Patient 1 is not available. Exiting.... ");
			 exit(1);
		 }
	  else
	     {
			 printf("Phase 2: The requested appointment is available and reserved to Patient1. The assigned doctor port number is %s.\n",avcheck);
		     close(sockfd);
		  }
	  	 
   //****************************End of  RECV the available Msg**********************************  	


//############################Phase 3#####################################

//***********************************Build the UDP with Doc*********************************	
	int usockfd;
	struct addrinfo uhints, *uservinfo, *up;
	char docpn[6];
	strncpy(docpn,avcheck,5);
	docpn[5]='\0';
	memset(&uhints, 0, sizeof uhints);
	uhints.ai_family = AF_UNSPEC;
	uhints.ai_socktype = SOCK_DGRAM;
	if (( getaddrinfo(hostname, docpn, &uhints, &uservinfo)) != 0) 
		{
		   fprintf(stderr, "getaddrinfo");
		   return 1;
	    }

	for(up = uservinfo; up != NULL; up = up->ai_next) 
	{
		if ((usockfd = socket(up->ai_family, up->ai_socktype,
				up->ai_protocol)) == -1) 
		{
			perror("patient1: socket");
			continue;
		}
     break;
     }
     
	if (p == NULL) 
	{
		fprintf(stderr, "patient1: failed to bind socket\n");
		return 2;
	}     
     	
//***********************************Build the UDP with Doc*********************************	

//*************************************Send the first MSG***********************************
        FILE *fp1;
        char insur[12];
        
        if ((fp1=fopen("patient1insurance.txt","r"))==NULL)
           {
			   printf("Cannot open the insurance file.\n");
			   exit(1);
		   }
		 else
		    {
				fgets(insur,11,fp1);
				insur[11]='\0';
			}
       
		if ( (sendto(usockfd, insur, 11, 0,
			 up->ai_addr, up->ai_addrlen)) == -1) 
			 {
		         perror("talker: sendto");
		         exit(1);
	          }

//*************************************End of the first MSG*********************************8
	
        //***************************Get Port Number*********************************
        struct sockaddr_in usockpn;
        socklen_t ulen = sizeof(usockpn);
        getsockname(usockfd,(struct sockaddr *)&usockpn,&ulen);
        int uppn = ntohs(usockpn.sin_port);
        printf ("Phase 3: Patient 1 has a dynamic UDP port number %d and IP address %s .\n", uppn, str4);
         //***************************Get Port Number*********************************

	
//**********************************Receive the MSG************************************
    struct sockaddr_storage udptheir_addr;
	socklen_t udpaddr_len;    
	char *udpipaddr;
    char recmsg[3];
    char docname[6];
    int flag1=1;
	while(flag1){
	if ((numbytes = recvfrom(usockfd, recmsg, 3 , 0,
		 (struct sockaddr *)&udptheir_addr, &udpaddr_len)) == -1) 
		 {
		    perror("recvfrom");
		    exit(1);
	     }
	if ((numbytes = recvfrom(usockfd, docname, 5 , 0,
		 (struct sockaddr *)&udptheir_addr, &udpaddr_len)) == -1) 
		 {
		    perror("recvfrom");
		    exit(1);
	     }
    struct sockaddr_in *udps=(struct sockaddr_in *)&udptheir_addr;
    udpipaddr=inet_ntoa(udps->sin_addr);
    printf ("Phase 3: The cost estimation request from Patient1 with insurance plan %s has been sent to the doctor with port number %s and IP address %s .\n",insur,docpn,udpipaddr);   
     docname[4]='\0';
    printf("Phase 3: Patient 1 receives %s $ estimation cost from doctor with port number %s and name %s . ", recmsg,docpn,docname);
    printf("\n");
    flag1=0;
    printf("Phase 3: End of Phase 3 for Patient 1 .\n");
    
    close(usockfd);
}


//**********************************End of Receive MSG*********************************
	
	return 0;
}

