/* This a peer_2 UDP code for peer-to-peer authentication. Last Modified on :11/5/2013 at 10:14 AM
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
#include <signal.h>

//Declarations for the size and port number
#define SERVERPORT "5438" 
#define MAXBUFLEN 1000
#define PEER_2_PORT "22438"
#define BACKLOG 10
#define MAXDATASIZE 1000

// Function to get the address of the local machine
void *get_in_addr(struct sockaddr *sa)
{
if (sa->sa_family == AF_INET) {
return &(((struct sockaddr_in*)sa)->sin_addr);
}
return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void sigchld_handler(int s)
{
while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void)
{
		int sockfd;
		struct addrinfo hints, *clientinfo, *p, *serverinfo;
		char *pch;
		int count;
		int rv;
		void *addr;
		void *port;
		int i;
		int numbytes;
		FILE *ptr_file;
		char filebuffer[MAXBUFLEN];
		char pwd[MAXBUFLEN];
		char peername[MAXBUFLEN];
		char ipstr[INET6_ADDRSTRLEN];
		char s[INET6_ADDRSTRLEN];
		char buf[MAXBUFLEN];
		struct sockaddr_storage their_addr;
		socklen_t addr_len;
		struct sockaddr_in myaddr;         
		int len; 
		char data[MAXBUFLEN];
		char port_no[MAXBUFLEN];
		int port_no_int;
	
		
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		
		// Getting the address details of the peer
		if ((rv = getaddrinfo("localhost",NULL, &hints, &clientinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		
		for(p = clientinfo; p != NULL; p = p->ai_next) 
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
			
		
		if (bind(sockfd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0) 
		{
				perror("bind failed");
				exit(1);
		}
		len = sizeof(myaddr);
		if (getsockname(sockfd, (struct sockaddr *)&myaddr, &len) < 0) 
		{
				perror("Error getting socketname \n");
				exit(1);
		}
		// Getting the dynamic port number of the port of the peer
		port_no_int =ntohs(myaddr.sin_port);
		sprintf(port_no, "%d",port_no_int);
		
		// Getting the address details of the server
		printf("Peer 2 Phase 1: The dynamic UDP port number:%d and IP address:%s \n",ntohs(myaddr.sin_port),ipstr);
		if ((rv = getaddrinfo("localhost",SERVERPORT, &hints, &serverinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
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
			
		// Reading the file and copying over the required details for authentication message
		
		ptr_file = fopen("peer_2.txt", "r");
		if (!ptr_file) return 1;
		memset(filebuffer, 0, sizeof filebuffer);
		memset(peername, 0, sizeof peername);
		fgets(filebuffer,1000,ptr_file);
		pch = strtok(filebuffer," ");
		count=0;
			while(pch != NULL)
			{
				
				if(strcmp(pch,"m") != 0)
				{
					if(strcmp(pch,"w") != 0)
					{
					if(count != 0)
					strcat(peername," ");
					strcat(peername,pch);
					}
					
				}
				pch = strtok(NULL," ");
				count++;
				
			
			}
		
			printf("%s Phase 1: Sending the authentication information to the server %s\n",strtok(filebuffer," "),peername);
			
			// Sending and receiving the authentication information and result
			if (sendto(sockfd,peername, sizeof peername,0,p->ai_addr, p->ai_addrlen) == -1) 
			{
				perror(" Unable to send the data\n");
				exit(1);
			}
			addr_len = sizeof their_addr;
			if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
			{
					perror("Unable to receive data \n");
					exit(1);
			}
			buf[numbytes] = '\0';
			if(strcmp(buf,"ACK") == 0)
			{
				printf("%s Phase 1: Authentication successful\n", strtok(filebuffer," "));
				memset(filebuffer, 0, sizeof filebuffer);
				memset(data, 0, sizeof data);
				ptr_file = fopen("peer_2.txt", "r");
				if (!ptr_file) return 1;
				fgets(filebuffer,1000,ptr_file);
				strcpy(pwd,filebuffer);
				//printf("The contents on file buffer is %s\n", filebuffer);
				// Constructing the token by parsing through each element of the string
				strcat(data, strtok(filebuffer," "));
					strcat(data," ");
					strcat(data,ipstr);
					strcat(data," ");
					strcat(data,PEER_2_PORT);
					strcat(data," ");
					//printf("The contents on file buffer 1 is %s\n", filebuffer);
					fgets(filebuffer,1000,ptr_file);
					//printf("The contents on file buffer 2 is %s\n", pwd);
					if(strstr(pwd,"m") != NULL)
						{
							strcat(data,"Mac");
						}
						else 
						{
							strcat(data,"Windows");
						}
					printf("%s Phase 1: Sending the file transfer information to the server <%s>\n",strtok(filebuffer," "),data);
				
					if (sendto(sockfd,data, sizeof data,0,p->ai_addr, p->ai_addrlen) == -1) 
					{
						perror("talker: sendto");
						exit(1);
					}
					if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
					{
					perror("recvfrom");
					exit(1);
					}
					printf(" %s Phase 1: Information received by the server\n", strtok(filebuffer," "));
			//printf("listener: got packet from %s\n",inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s));
			//printf("listener: packet is %d bytes long\n", numbytes);
			buf[numbytes] = '\0';
			//printf("listener: packet contains \"%s\"\n", buf);
			freeaddrinfo(clientinfo);
				close(sockfd);
				//return 0;
				
			}
			// If we get a NAK, client will tear down the connection
			else if(strcmp(buf,"NAK") == 0)
			{
				printf("%s Phase 1: Authentication unsuccessful\n", peername);
				freeaddrinfo(clientinfo);
				close(sockfd);
				//return 0;
				
			}
			printf("%s Phase 1: End of Phase 1\n",strtok(filebuffer," "));
			printf("%s Phase 3: The %s has static TCP port number %s and IP address %s\n",strtok(filebuffer," "),strtok(filebuffer," "),PEER_2_PORT,ipstr);

			int new_fd;			
			struct addrinfo *servinfo;
			int yes = 1;
			struct sigaction sa;
			socklen_t sin_size;
			char *peer;
			char *ip;
			char *port_msg;
			char *sender;
			char temp[MAXBUFLEN];
			char message[MAXBUFLEN];
			char *machine_type;
			int count_token = 0;
			char buf1[MAXBUFLEN];
			

			memset(&hints, 0, sizeof hints);
			memset(message, 0, sizeof message);
			memset(temp, 0, sizeof temp);
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_flags = AI_PASSIVE; 		
		
		if ((rv = getaddrinfo("localhost",PEER_2_PORT, &hints, &servinfo)) != 0) 
		{
			fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
			return 1;
		}
		for(p = servinfo; p != NULL; p = p->ai_next)
	       	{
			if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
			{
					perror("server: socket");
					continue;
			}
			if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) 
			{
				perror("setsockopt");
				exit(1);
			}
			if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
			{
				close(sockfd);
				perror("server: bind");
				continue;
			}
			break;
		}
		if (p == NULL)  
		{
			fprintf(stderr, "server: failed to bind\n");
			return 2;
		}
		
		if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
		}

		sa.sa_handler = sigchld_handler; 
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &sa, NULL) == -1) 
		{
			perror("sigaction");
			exit(1);
		}
		//printf("peer 2: waiting for connections...\n");
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) 
		{
			perror("accept");
			
		}

		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
		//printf("peer 2: got connection from %s\n", s);
		if (!fork()) 
		{ 
			close(sockfd); 
			if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) 
			{
		    		perror("recv");
		    		exit(1);
			}
			buf[numbytes] = '\0';
			strcpy(temp,buf);
			sender = strtok(buf," ");
			peer = strtok(NULL," ");
			ip = strtok(NULL," ");
			port_msg = strtok(NULL," ");
			machine_type = strtok(NULL, " ");
			printf("%s Phase 3: Received the file from %s \n",strtok(filebuffer," "),sender); 
			//printf("The peer:%s and IP: %s and port: %s\n", peer,ip,port_msg);
			//printf("Temp contents are: '%s'\n",temp);
			//printf("Buf contents are: '%s'\n",buf);
			strcpy(buf1,temp);
			strcat(message,"peer2");
			strcat(message," ");
			pch = strtok(buf1," ");
			while(pch != NULL)
			{
				//printf( "The token is : %s\n",pch);
				count_token++;
				if(count_token > 5)
				{
					//printf( "Inside the loop token is : %s\n",pch);
					strcat(message,pch);
					strcat(message," ");
					
				}
    			        pch = strtok(NULL," ");
			}
			//printf("The counter is %d\n", count_token);
			//printf("The message is %s\n", message);
			close(new_fd);
			//printf("%s Phase 3: The contents of the file are as follows : %s \n",strtok(filebuffer," "), temp);
			if(count_token >= 5)
			{
				printf("%s Phase 3: The contents of the file are as follows : %s \n",strtok(filebuffer," "), temp);
			}
			else
			{
				printf("%s Phase 3: There are no more contents in the file\n",strtok(filebuffer," "));
			}
		if(count_token >= 5)
		{
		
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
				printf("%s Phase 3: The peer has dynamic TCP port number:%d and IP address:%s\n",strtok(filebuffer," "),ntohs(myaddr.sin_port),ipstr);
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		if ((rv = getaddrinfo(ip,port_msg, &hints, &servinfo)) != 0) 
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
		//printf("peer_2: connecting to %s\n", s);
		freeaddrinfo(servinfo);
	printf("%s Phase 3: Sending file to %s having IP address %s and static TCP port number %s\n",strtok(filebuffer," "),peer,ip,port_msg);
		if (send(sockfd, message, sizeof message, 0) == -1)
		{
			perror("Send Error");
		}
	}	
			printf("%s Phase 3: File Transfer successful\n",strtok(filebuffer," "));
			printf("%s Phase 3: End of Phase 3\n",strtok(filebuffer," "));
			close(new_fd);
			exit(0);
		}
		close(new_fd);  
		return 0;
			
			
}
