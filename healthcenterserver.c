/*
** server.c -- a stream socket server demo
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#define PORT "21799" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold


void sigchld_handler(int s) //This  function is a copy of Beej's guide book
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)   //This get_in_addr function is a copy of Beej's guide book
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
int main(void)
{
        FILE *fp2;
        if ((fopen("reserve.txt","r"))==NULL)
          {
              fp2=fopen("reserve.txt","w+");
              fclose(fp2);
          }

        char *username;
        // **************************Get hostname and IP address************************** 
        char **pptr;
        char str4 [INET_ADDRSTRLEN];
        char str6 [INET6_ADDRSTRLEN];
        int flag=1;
        struct hostent *hip;
        char hostname [128];
        gethostname (hostname,sizeof (hostname));
        //printf("hostname is : %s\n",hostname);
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

        //*******************************Print first Msg************************************
        if (flag)              //flag is used to indicate if the addr is IPV4 or IPV6
        {
          printf("Phase 1: The Health Center Server has port number %s and IP address %s .\n", PORT,str4);
        }
        else
        {
          printf("Phase 1: The Health Center Server has port number %s and IP address %s .\n", PORT,str6);
        }          
          
        // ********************************End of First Msg*********************************



        // ********************************Read the file*********************************
        FILE *fp;
        char content1[21];
        char content2[21];
        char hh[1];//THis is to tear the info of 2 patients apart
        char *name1,*psw1;
        char *name2,*psw2;
 
        if((fp=fopen("users.txt","r"))==NULL) // Open the file and exit if fail
          {
            printf("Cannot open file\n");
            exit(1);
          }
        fgets(content1,21,fp);
        fread(hh,1,1,fp);
        fread(content2,21,1,fp);
        content2[20]='\0';
        name1=strtok(content1," ");
        psw1=strtok(NULL," ");
        name2=strtok(content2," ");
        psw2=strtok(NULL," "); 

        // ********************************End of read file*********************************



		int sockfd, new_fd; // listen on sock_fd, new connection on new_fd
		struct addrinfo hints, *servinfo, *p;
		struct sockaddr_storage their_addr; // connector's address information
		socklen_t sin_size;
		struct sigaction sa;
		int yes=1;
		char s[INET6_ADDRSTRLEN];
		int rv;
		int pport;
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE; // use my IP
		if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
		}
		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
		p->ai_protocol)) == -1) {
		perror("server: socket");
		continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
		sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		perror("server: bind");
		continue;
		}
		break;
		}
		if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
		 }
		freeaddrinfo(servinfo); // all done with this structure
		if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
		}
		sa.sa_handler = sigchld_handler; // reap all dead processes
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
		}
		while(1) { // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
		perror("accept");
		continue;
		}
		inet_ntop(their_addr.ss_family,
		get_in_addr((struct sockaddr *)&their_addr),
		s, sizeof s);
		struct sockaddr_in *paddr=(struct sockaddr_in*)&their_addr;
		pport=ntohs(paddr->sin_port);
		
		if(!fork())
		{
		//****************************Receive the Msg*******************************************
		char buff1[40];
		char *rec1;
		char *rec2;
		char *rec3;
		char *reply="unknown";
		char recpsw[4];
		int k;
		
		if((recv(new_fd,buff1,sizeof buff1,0))==-1)
		{
		  perror("recv");exit(1);
		}
		rec1=strtok(buff1," ");
		rec2=strtok(NULL," ");
		rec3=strtok(NULL," ");
		
		//*************************************************Get the Peer Addr**********************************
		
		//*************************************************End of Peer Addr**********************************
		
		
		if ((strcmp(rec1,"authenticate"))!=0)
		{
		  reply="failure";
		}
		else
		{
		
		  for(k=0;k<=2;k++)
		   {
		     recpsw[k]=rec3[8+k];
		   }
		  recpsw[3]='\0';
		  printf("Phase 1: The Health Center Server has received request from a patient with username %s and password %s .\n",rec2, recpsw);
		  
		  if ((strcmp(rec2,name1))==0)
		   {
		     if ((strcmp(rec3,psw1))==0){reply="success";}
		     else{reply="failure";}     
		   }
		  else if ((strcmp(rec2,name2))==0)
		   {
		     if ((strcmp(rec3,psw2))==0){reply="success";}
		     else{reply="failure";}     
		   }
		
		}
		if (send(new_fd, reply, sizeof reply, 0) == -1)
		perror("send");
		printf("Phase 1: The Health Center Server sends the response %s to patient with username %s .\n", reply, rec2);//Print the response to user client
//****************************End of Receiving*******************************************

//###################################Phase 2####################################


//******************************************Read File***********************************************
        FILE *fp1,*fp3;
        char avindex[150];
        char avindex1[72];
        char   *index[6];
        char   index1[6][11];     
        int i;
		int j;
        int reserve2;

        fp3=fopen("reserve.txt","r");	
		fscanf(fp3,"%d",&reserve2);
        fclose(fp3);
        
        index[0] = (char *)malloc(sizeof(char) * 6 * 22); //allocate space before use
       
        if((fp1=fopen("availabilities.txt","r"))==NULL) // Open the file and exit if fail
          {
            printf("Cannot open file\n");
            exit(1);
          }
		  
 
        fread(avindex,131,1,fp1);
        index[0]=strtok(avindex,"\n");
        index[0][21]='\0';
 
         for (i=1;i<=5;i++)
             {
				index[i]=strtok(NULL,"\n");
			  }

        memset(avindex1,0,72); //clear before use
 
        for (i=0;i<=5;i++)
	          {
			     for(j=0;j<=9;j++)
				   {
				      index1[i][j]=index[i][j];
				   }
			  index1[i][10]='\n';
			  index1[i][11]='\0';
			  if((i+1)!=reserve2)
			    {
			        strcat(avindex1,index1[i]);
		         }

			   } 

//*****************************************End of Read File******************************************


//*******************************************Receive availability*****************************************

if (((strcmp(reply,"success"))==0)&&((strcmp(rec2,name1))==0))
    {
        username=name1;
        username[8]='\0';
		char avai1[10];
		if((recv(new_fd,avai1,sizeof avai1,0))==-1)
          {
                perror("recv");
                exit(1);
           }

      
	   if ((strcmp(avai1,"available\0"))==0)
	       {
               printf("Phase 2: The Health Center Server, receives a request for available time slots with port number %d and IP address %s .\n",pport,s); 
               
        	   if((send(new_fd,avindex1,sizeof avindex1,0))==-1)
			   perror("send availability");
			   printf("Phase 2: The Health Center Server sends available time slots to patient with user name %s .\n",name1);
		   }
		else{puts(avai1);}   
		} 
		

if (((strcmp(reply,"success"))==0)&&((strcmp(rec2,name2))==0))
    {
        username=name2;
        username[8]='\0';
		char avai2[10];
		if((recv(new_fd,avai2,sizeof avai2,0))==-1)
          {
                perror("recv");exit(1);
           }

	   if ((strcmp(avai2,"available\0"))==0)
	       {
			   printf("Phase 2: The Health Center Server, receives a request for available time slots with port number %d and IP address %s .\n",pport,s); 
			   if((send(new_fd,avindex1,sizeof avindex1,0))==-1)
			   perror("send availability");
			   printf("Phase 2: The Health Center Server sends available time slots to patient with user name %s .\n",name2);
		   }	
	} 




//*******************************************End of Receive*********************************************

//********************************************Check Availability****************************************
		char indexsel[11];
		char *sel;
		char *selnum;
		char *docname;
		int reserveatt=0;
		int reserve1;
	


while  ((recv(new_fd,indexsel,sizeof indexsel,0))<11) //Use loop mechanism to wait for the selection of patient
     {
	 }
    sel=strtok(indexsel," ");
    if ((strcmp(sel,"selection"))==0)
    {
		selnum=strtok(NULL," ");
		reserveatt=selnum[0]-'0';

         fp2=fopen("reserve.txt","r");	
		fscanf(fp2,"%d",&reserve1);
        fclose(fp2);
        
 
		if (reserveatt==reserve1)
		    {
				send(new_fd,"notavailable",12,0);				
			}
		else
		    {
                if(reserveatt==6)
                {
					char *index3="6 Fri 04pm doc2 42799\0";
					printf("Phase 2: The Health Center Server sends the following appointment %s to patient with username %s .\n",index3,username);
				    char *docname1="42799\0";
                    send(new_fd,docname1,10,0); 
				}
				else
				{
				printf("Phase 2: The Health Center Server sends the following appointment %s to patient with username %s .\n",index[reserveatt-1],username);
			    fp2=fopen("reserve.txt","w+");
				fprintf(fp2,"%d",reserveatt);
				fclose(fp2);
				index[5]="6 Fri 04pm doc2 42799\0";
		        docname=strtok(index[reserveatt-1]," ");
		        docname=strtok(NULL," ");
		        docname=strtok(NULL," ");
		        docname=strtok(NULL," ");
		        docname=strtok(NULL," ");
     			send(new_fd,docname,10,0);
			    }
			}
	}
}
//******************************************End of Check*********************************************
}
return 0;
}
