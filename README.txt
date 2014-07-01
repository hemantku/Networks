Full Name: Vineet Ghatge	
USCID: 7068085338


Compilation and Execution Commands:-
1. Please open a terminal and run the following command - gcc -o server server.c -lsocket -lnsl -lresolv
2. Run the command in a terminal - gcc -o peer_1 peer_1.c -lsocket -lnsl -lresolv
3. Run the command in a terminal - gcc -o peer_2 peer_2.c -lsocket -lnsl -lresolv
4. Run the command in a terminal - gcc -o peer_3 peer_3.c -lsocket -lnsl -lresolv
5. Run the command in a terminal - gcc -o peer_4 peer_4.c -lsocket -lnsl -lresolv
6. Execute all the executable starting from the server
7. Alternatively, you can run the make file for automated compilation


Assumptions and Idiosyncrasies:-
1. The entries in the files are of the format of the following 
	-Peer sample is Peer1 m Pass1
	-Server sample is Peer1 Pass1
2. The file output - content_mac.txt and content_win.txt  are created for the first run and are not purged for other iterative runs. 
The data will be appended to existing file.
3. There are no \t spaces between content of the same file.
4. Its is assumed that neither the peername nor the password have a letter 'w' or 'm' in the text. These will act as reserved words for identifying Mac and windows. There no other opearting systems other than these two and a system cannot exist without any opearting system/
5. There are no peers who do not have an operating systems.
6. We are displaying the peernames and not a particular user - Harshad as Peer1. This is done for better mapping and extensibility purposes.
7. The data out on the command prompt is where the contents do not have an extra space.

Sources used:
Beej's Guide and http://www.cplusplus.com/reference/cstring/strcmp/
