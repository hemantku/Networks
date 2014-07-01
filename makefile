all:server peer_1 peer_2 peer_3 peer_4 mirror
server:server.c
	/usr/usc/bin/gcc -o server server.c -lsocket -lnsl -lresolv
peer_1:peer_1.c
	/usr/usc/bin/gcc -o peer_1 peer_1.c -lsocket -lnsl -lresolv
peer_2:peer_2.c
	/usr/usc/bin/gcc -o peer_2 peer_2.c -lsocket -lnsl -lresolv
peer_3:peer_3.c
	/usr/usc/bin/gcc -o peer_3 peer_3.c -lsocket -lnsl -lresolv
peer_4:peer_4.c
	/usr/usc/bin/gcc -o peer_4 peer_4.c -lsocket -lnsl -lresolv 
mirror:mirror.c
	/usr/usc/bin/gcc -o mirror mirror.c -lsocket -lnsl -lresolv 
