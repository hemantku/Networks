/* This a server UDP code for peer-to-peer authentication. Last Modified on :11/11/2013 at 10:26 PM
Author: Vineet Ghatge*/
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

//Declarations for the size and port number, peers
#define MAXBUFLEN 100
#define SERVERPORT "5438"

// Function to get the address of the local machine
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int main(void)
{
		int sockfd;
		int sockfd_port;
		struct addrinfo hints, *mirrorinfo,*p, *serverinfo;
		int rv;
		void *addr;
		char ipstr[INET6_ADDRSTRLEN];
		char buf[MAXBUFLEN];
		struct sockaddr_in myaddr;         
		int len; 
		char port_no[MAXBUFLEN];
		char filebuffer[MAXBUFLEN];
		char s[INET6_ADDRSTRLEN];
		int port_no_int;
		FILE *ptr_file;
		int numbytes;
		struct sockaddr_storage sender_addr;
		socklen_t addr_len;
		size_t nread;
		int count_content=0;
		char *port;
		char *peer;
		char *ip;
		struct addrinfo *servinfo;
		char message[1000];
		
		// To get the host name of the server
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		
		// Getting the address details of the peer
		if ((rv = getaddrinfo("localhost",NULL, &hints, &mirrorinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		
		for(p = mirrorinfo; p != NULL; p = p->ai_next) 
		{
			if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
			{
				perror("Failed to get a socket");
				continue;
			}
			break;
		}
		if (p == NULL) 
		{
				fprintf(stderr, "Failed to bind socket");
				return 2;
		}
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
			
		
		len = sizeof(myaddr);
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = 0;
		bind(sockfd,(struct sockaddr *)&myaddr,len);
		
		if (getsockname(sockfd, (struct sockaddr *)&myaddr, &len) < 0) 
		{
				perror("Error getting socketname \n");
				exit(1);
		}
		// Getting the dynamic port number of the port of the peer
		port_no_int =ntohs(myaddr.sin_port);
		sprintf(port_no, "%d",port_no_int);
		
		// Getting the address details of the server
		printf("Mirror Phase 2: The dynamic UDP port number:%d and IP address:%s \n",ntohs(myaddr.sin_port),ipstr);
		if ((rv = getaddrinfo("localhost",SERVERPORT, &hints, &serverinfo)) != 0) 
		{
			fprintf(stderr, "Getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		
		for(p = serverinfo; p != NULL; p = p->ai_next) 
		{
			if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
			{
				perror("Failed to get a socket");
				continue;
			}
			break;
		}
		if (p == NULL) 
		{
				fprintf(stderr, "Failed to bind socket");
				return 2;
		}
		ptr_file = fopen("mirror.txt","r");
		if (!ptr_file) return 1;
		memset(filebuffer, 0, sizeof filebuffer);
		fgets(filebuffer,100,ptr_file);
		printf("Mirror Phase 2: Sending the authentication information to the server %s \n",filebuffer); 
		if (sendto(sockfd,filebuffer, sizeof filebuffer,0,p->ai_addr, p->ai_addrlen) == -1) 
		{
				perror(" Unable to send the data\n");
				exit(1);
		}
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&sender_addr, &addr_len)) == -1) 
		{
					perror("recvfrom");
					exit(1);
		}
		//Authentication information being sent from mirror to server
		buf[numbytes] = '\0'; 
		fclose(ptr_file);
		if(strcmp(buf,"ACK") == 0)
		{
			printf("Mirror Phase 2: Authentication successful\n");
			printf("Mirror Phase 2: Seeking the information about the Mac Clients \n");
			if (sendto(sockfd,"Mac", sizeof("Mac"),0,p->ai_addr, p->ai_addrlen) == -1) 
			{
				perror(" Unable to send the data\n");
				exit(1);
			}
			//printf("I am here\n");
			if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&sender_addr, &addr_len)) == -1) 
			{
					perror("recvfrom");
					exit(1);
			}
			//printf("I am here\n");
			buf[numbytes] = '\0'; 
			//ptr_file = fopen(buf,"r");  //File Name is being used get the contents of the Mac file
			if (ptr_file = fopen(buf,"r"))
			{
				printf("Mirror Phase 2: File Name received from the server. The Contents of the file are as follows:\n"); 
				while ((nread = fread(filebuffer, 1, sizeof filebuffer,ptr_file)) > 0)
				{
					fwrite(filebuffer, 1, nread, stdout);
				}
				fclose(ptr_file);
							
			}
			else
			{
				printf("Mirror Phase 2: File - content_mac.txt does not exist\n");
			}


			
		
		}
		else if(strcmp(buf,"NAK") == 0)
		{
			printf("Mirror Phase 2:Authentication unsuccessful\n");
				
		}
		printf("Mirror Phase 2: End of Phase 2\n"); 
		freeaddrinfo(mirrorinfo);
		close(sockfd);
		
		
		// Getting the address details of the peer
		if ((rv = getaddrinfo("localhost",NULL, &hints, &servinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		
		for(p = servinfo; p != NULL; p = p->ai_next) 
		{
			if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
			{
				perror("Failed to get a socket");
				continue;
			}
			break;
		}
		if (p == NULL) 
		{
				fprintf(stderr, "Failed to bind socket");
				return 2;
		}
					
		len = sizeof(myaddr);
		myaddr.sin_family = AF_INET;
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		myaddr.sin_port = 0;
		bind(sockfd,(struct sockaddr *)&myaddr,len);
		
		if (getsockname(sockfd, (struct sockaddr *)&myaddr, &len) < 0) 
		{
				perror("Error getting socketname \n");
				exit(1);
		}
		printf("Mirror Phase 3: Dynamic TCP port number:%d and IP address:%s\n",ntohs(myaddr.sin_port),ipstr);
		ptr_file = fopen("content_mac.txt","r");
		if(ptr_file != NULL)
		{
			while ((nread = fread(filebuffer,1, sizeof filebuffer,ptr_file)) > 0)
			{
				count_content++;
				//fwrite(filebuffer, 1, nread, stdout);
								
				
			}
               		//printf("The contents of the string is %s\n", str);
			//printf("The count_content is %d\n",count_content - 1);
			printf("Server Phase 3: There are %d Mac Clients \n",count_content - 1);
			fclose(ptr_file);
			ptr_file = fopen("content_mac.txt","r");
			fgets(filebuffer,sizeof filebuffer,ptr_file);
			//printf("The contents of the file buffer is %s\n", filebuffer);
			peer = strtok(filebuffer," ");
                	ip = strtok(NULL," ");
                	port = strtok(NULL," ");
                	//printf("The peer:%s and IP: %s and port: %s\n", peer,ip,port);
                	printf("Mirror Phase 3: Sending file to %s having IP address %s and static TCP port number %s\n",peer,ip,port);
			memset(&hints, 0, sizeof hints);
			hints.ai_family = AF_UNSPEC;
			hints.ai_socktype = SOCK_STREAM;
			if ((rv = getaddrinfo(ip,port, &hints, &servinfo)) != 0) 
			{
				fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
				return 1;
			}
			for(p = servinfo; p != NULL; p = p->ai_next) 
			{
				if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
				{
					perror("client: socket");
					continue;
				}

				if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
				{
					close(sockfd);
					perror("client: connect");
					continue;
				}
				break;
			}
	
			if (p == NULL) 
			{
				fprintf(stderr, "client: failed to connect\n");
				return 2;
			}

			inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
			//printf("client: connecting to %s\n", s);
			freeaddrinfo(servinfo); 
	
			memset(message, 0, sizeof message);
			strcat(message, "mirror");
			while(fgets(filebuffer,sizeof filebuffer,ptr_file) != NULL)
			{
				//printf("The contents of the file buffer is %s\n", filebuffer);
				//printf("The contents of the file buffer 2 is %s\n", filebuffer);
				if(strcmp (filebuffer," ") !=0)
				{			
					strcat(message, filebuffer);
					strcat(message," ");
				}
	
			}
			//printf("The contents of the message is %s\n",message);
			printf("Mirror Phase 3: File Transfer successful\n");
			if (send(sockfd, message, sizeof message, 0) == -1)
			perror("Send Error");
			printf("Mirror Phase 3: End of Phase 3\n");
		}
		else
		{
			printf("Mirror Phase 3: File - content-mac.txt does not exist!\n");
			printf("Mirror Phase 3: End of Phase 3\n");
		}

	return 0;
}
		

