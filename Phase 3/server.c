/* This a server UDP code for peer-to-peer authentication. Last Modified on :11/5/2013 at 10:02 AM
Author: Vineet Ghatge*/

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <sys/wait.h>

//Declarations for the size and port number, peers
#define MAXBUFLEN 100
#define STATIC_PORT "5438"
#define NO_OF_PEERS 4
#define PEER_1_PORT "21438"


 
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
	struct addrinfo hints, *servinfo, *p;
	int rv;
	void *addr;
	//char hostname[128];
	int numbytes;
	int flag1 = 0;
	int flag2 = 0;
	char *dt;
	char ipstr[INET6_ADDRSTRLEN];
	char pwd[MAXBUFLEN];
	char pwd1[MAXBUFLEN];
	char s[INET6_ADDRSTRLEN];
	char buf[MAXBUFLEN];
	struct sockaddr_storage their_addr;
	socklen_t addr_len;
	FILE* ptr_file;
	FILE *file;
	char filebuffer[MAXBUFLEN];
	int i;
	size_t nread;
	int count_content=0;
	
	//memset(hostname, 0, sizeof hostname); // To get the host name of the server
	remove("content_win.txt");
	remove("content_mac.txt");
	memset(&hints, 0, sizeof hints);
	memset(filebuffer, 0, sizeof filebuffer);
	memset(buf,0, sizeof buf);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; 
	
	// Getting the address of the server
	if ((rv = getaddrinfo("localhost",STATIC_PORT,&hints,&servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	
	//Map the socket address to the respective fields
	for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
		{	
			perror("Failed to get a socket\n");
			continue;
		}
		if (bind(sockfd, p->ai_addr,p->ai_addrlen) == -1) 
		{
			close(sockfd);
			perror("Failed to bind a socket\n");
			continue;
		}
		break;
		
	}
	
	if (p == NULL) 
	{
		fprintf(stderr, "Failed to bind socket\n");
		return 2;
	}
	
	// Getting the IP address for the current server: nunnki.usc.edu
	struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
	addr = &(ipv4->sin_addr);
	inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
	
for ( i= 0; i< NO_OF_PEERS; i++)
{	

	printf("Server Phase 1: The server has static UDP port number :%s and IP address: %s\n",STATIC_PORT, ipstr);
	
		// Waiting to hear back from the client via the UDP socket
		addr_len = sizeof their_addr;
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
		{
					perror("Failed to Receive from \n");
					exit(1);
		}
		buf[numbytes] = '\0';
		
		/* Opening the file server.txt to get the data to match data from packet. 
			If the authentication is successful we set the flag to 1 else to 0
			ACK will be sent based on the flag which is set. We send a negative ACK if the authentication does not go through*/
			
		ptr_file = fopen("server.txt","r");
		while(fgets(filebuffer,100, ptr_file)!= NULL)
		{
			//printf("The flag value is %d\n", flag1);
			//printf("The filebuffer data is  value is %s\n", filebuffer);
			//printf("The filebuffer data is  value is %s\n", buf);
			if(strcmp(filebuffer,buf) == 0)
			{
				flag1 = 1;
				//printf("The filebuffer2 data is  value is %s\n", filebuffer);
				//printf("The filebuffer2 data is  value is %s\n", buf);
				break;
			}
			//printf("The flag value 2 is %d\n", flag1);
		}	
		if (flag1 == 1) 
			{
					printf("Server Phase 1: %s  authentication successful, sending ACK\n", strtok(buf," "));
					if(sendto(sockfd,"ACK",sizeof("ACK"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
					{
						perror("Failed to send ACK Packet\n");
						exit(1);
					}
					flag1=0; // This gets set from other peers
					
					/* Wait for the client to send the data with its details.  
			Write the data into two separate files having windows and Mac clients 
			Tokenize each string variable and move them into the buffer space before moving them into the file.
			Tear down connection and send ACK.
		*/
		
		if((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1)
		{
					perror("Failed to receive bytes of data\n");
					exit(1);
		}
		//printf("listener: packet contains \"%s\"\n", buf);
		buf[numbytes] = '\0';
		memset(pwd, 0 , sizeof pwd);
                memset(pwd1, 0 , sizeof pwd1);
		strcpy(pwd,buf);
		strcpy(pwd1,buf);
		printf("Server Phase 1: %s information received <%s>\n",strtok(buf," "),pwd);
		pwd[numbytes] = '\0';
		//printf("The PWD value is %s\n", pwd);
		dt = strtok(pwd," ");
		//printf("The PWD1 value is %s\n", pwd1);
		while(dt != NULL)
		{
				//printf(" I am here \n");
				if(strcmp(dt,"Windows") == 0)
				{
							FILE *file;
							int file_exists;
							file=fopen("content_win.txt","r");
							if (file==NULL) file_exists=0;
							else {file_exists=1; fclose(file);}
							if (file_exists==1)
							{

									file=fopen("content_win.txt","a+");
									//printf("The PWD value is %s\n", pwd);
									//printf("The PWD1 value is %s\n", pwd1);
									fwrite(pwd1,sizeof(char), sizeof(pwd1), file);
									fprintf(file,"\n");
									fclose(file);
							}
							else
							{
									file=fopen("content_win.txt","w+b");
									//printf("The PWD value is %s\n", pwd);
									//printf("The PWD1 value is %s\n", pwd1);
									fwrite(pwd1,sizeof(char),sizeof(pwd1), file);
									fprintf(file,"\n");
									fclose(file);
							}
				}
				else if (strcmp(dt,"Mac") ==0)
				{
							FILE *file;
							int file_exists;
							file=fopen("content_mac.txt","r");
							if (file==NULL) file_exists=0;
							else {file_exists=1; fclose(file);}
							if (file_exists==1)
							{
									
									file=fopen("content_mac.txt","a+");
									fwrite(pwd1,sizeof(char),sizeof(pwd1), file);
									fprintf(file,"\n");
									fclose(file);
							}
							else
							{
									file=fopen("content_mac.txt","w+b");
									fwrite(pwd1, sizeof(char),sizeof(pwd1), file);
									fprintf(file,"\n");
																		    
									fclose(file);
							}
							
				}
						dt = strtok(NULL," ");
		}
			printf("Information written to the respective files\n");
			if(sendto(sockfd,"ACK",sizeof("ACK"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
			{
						perror("Failed to send ACK packet\n");
						exit(1);
			}
		
					
			}
			else
			{
				printf("Server Phase 1: %s  authentication unsuccessful, sending NAK\n", strtok(buf," "));
				if(sendto(sockfd,"NAK",sizeof("NAK"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
				{
						perror("Failed to send NAK Packet\n");
						exit(1);
				}
			
			}
			
		

}
	fclose(ptr_file);	
	printf("Server Phase 1:End of Phase 1\n");
	printf("Server Phase 2: The server has static UDP port number :%s and IP address: %s\n",STATIC_PORT, ipstr);
	if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
		{
					perror("Failed to Receive from \n");
					exit(1);
		}
		buf[numbytes] = '\0';
		ptr_file = fopen("server.txt","r");
		while(fgets(filebuffer,100, ptr_file)!= NULL)
		{
			//printf("The contents in filebuffer %s \n", filebuffer);
			//printf("The contents in buffer %s \n", buf);
			if(strcmp(filebuffer,buf) == 0)
			{
				
				flag2 = 1;
				break;
				
			}
			
		}
		if(flag2 == 1)
		{
				printf("Server Phase 2:Mirror's authentication successful\n");
				if(sendto(sockfd,"ACK",sizeof("ACK"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
				{
						perror("Failed to send ACK packet\n");
						exit(1);
				}
				if ((numbytes = recvfrom(sockfd, buf, MAXBUFLEN-1 , 0,(struct sockaddr *)&their_addr, &addr_len)) == -1) 
				{
					perror("Failed to Receive from \n");
					exit(1);
				}
				buf[numbytes] = '\0';
				if(strcmp(buf,"Mac") == 0)
				{
										
					printf("Server Phase 2:Request received from the mirror, transferring the file name\n");
					if (file = fopen("content_mac.txt", "r"))
					{
						fclose(file);
						if(sendto(sockfd,"content_mac.txt",sizeof("content_mac.txt"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
						{
							perror("Failed to send ACK packet\n");
							exit(1);
						}
						printf("Server Phase 2: File Name was sent to the mirror\n");
					}
					else
					{
						printf("Server Phase 2: File does not exist!\n");
						//fclose(file);
						if(sendto(sockfd,"content_mac.txt",sizeof("content_mac.txt"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
						{
							perror("Failed to send ACK packet\n");
							exit(1);
						}
						
					
					}
					
				}
		
		}
		else
		{
				printf("Server Phase 2:Mirror's authentication unsuccessful\n");
				if(sendto(sockfd,"NAK",sizeof("NAK"),0,(struct sockaddr *)&their_addr, addr_len) == -1)
				{
						perror("Failed to send ACK packet\n");
						exit(1);
				}
		
		}
		printf("Server Phase 2: End of Phase 2\n");
		fclose(ptr_file);
		freeaddrinfo(servinfo);
		close(sockfd);
		//return 0;
		
		
		char *port;
		char *peer;
		char *ip;
		struct sockaddr_in myaddr;
		int len;
		
		char message[1000];
		
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
		printf("Server Phase 3: Dynamic TCP port number:%d and IP address:%s\n",ntohs(myaddr.sin_port),ipstr);
		ptr_file = fopen("content_win.txt","r");
		if(ptr_file != NULL)
		{
			while ((nread = fread(filebuffer,1, sizeof filebuffer,ptr_file)) > 0)
			{
				count_content++;
				//fwrite(filebuffer, 1, nread, stdout);
								
				
			}
               		//printf("The contents of the string is %s\n", str);
			//printf("The count_content is %d\n",count_content - 1);
			printf("Server Phase 3: There are %d Windows Clients \n",count_content - 1);
			fclose(ptr_file);
			ptr_file = fopen("content_win.txt","r");
			fgets(filebuffer,sizeof filebuffer,ptr_file);
			//printf("The contents of the file buffer is %s\n", filebuffer);
			peer = strtok(filebuffer," ");
                	ip = strtok(NULL," ");
                	port = strtok(NULL," ");
                	//printf("The peer:%s and IP: %s and port: %s\n", peer,ip,port);
                	printf("Server Phase 3: Sending file to %s having IP address %s and static TCP port number %s\n",peer,ip,port);
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
			strcat(message, "server");
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
		printf("Server Phase 3: File Transfer successful\n");
		if (send(sockfd, message, sizeof message, 0) == -1)
		perror("Send Error");
		printf("Server Phase 3: End of Phase 3\n");
	}
	else
	{
		printf("Server Phase 3: File - content-win.txt does not exist!\n");
		printf("Server Phase 3: End of Phase 3\n");
	}
	
	return 0;
}




