#include "pipe_networking.h"

/*=========================
  server_setup
    args:

    creates the WKP (upstream) and opens it, waiting for a
    connection.

    removes the WKP once a connection has been made

    returns the file descriptor for the upstream pipe.
=========================*/
int server_setup() {

    printf("[server] making wkp\n");
    // Old mkfifo had a pipe
    if (mkfifo(WELL_KNOWN_PIPE_NAME, 0644) == -1) {
        printf("Error creating from_clients pipe in server handshake!\n");
        exit(1);
    }

    printf("[server] SETUP: opening to_client file\n");
    // i-o-i-o-i (except in this case it's only i so uhhh yeah)
    int from_client = open(WELL_KNOWN_PIPE_NAME, O_RDONLY);

    printf("[server] SETUP: connection successful!\n");
    // and old mkfifo hated files
    // and the fifo died
    // the end
    remove(WELL_KNOWN_PIPE_NAME);

    return from_client;
}


/*=========================
  server_connect
args: int from_client

handles the subserver portion of the 3 way handshake

returns the file descriptor for the downstream pipe.
=========================*/
int server_connect(int from_client) {
    
    int to_client;

    char buffer[HANDSHAKE_BUFFER_SIZE];

    read(from_client, buffer, sizeof(buffer));
    printf("[subserver %d] handshake: received [%s]\n", getpid(), buffer);

    //    remove(WELL_KNOWN_PIPE_NAME);
    printf("[subserver %d] handshake: removed wkp\n", getpid());

    //connect to client, send message
    to_client = open(buffer, O_WRONLY, 0);
    write(to_client, buffer, sizeof(buffer));

    //read for client
    read(from_client, buffer, sizeof(buffer));
    printf("[subserver %d] handshake received: %s\n", getpid(), buffer);

    return to_client;
}

/*=========================
  server_handshake
args: int * to_client

Performs the server side pipe 3 way handshake.
Sets *to_client to the file descriptor to the downstream pipe.

returns the file descriptor for the upstream pipe.
=========================*/
int server_handshake(int *to_client) {

    int from_client;

    char buffer[HANDSHAKE_BUFFER_SIZE];

    mkfifo(WELL_KNOWN_PIPE_NAME, 0600);

    //block on open, recieve mesage
    printf("[server] handshake: making wkp\n");
    from_client = open( WELL_KNOWN_PIPE_NAME, O_RDONLY, 0);
    read(from_client, buffer, sizeof(buffer));
    printf("[server] handshake: received [%s]\n", buffer);

    remove(WELL_KNOWN_PIPE_NAME);
    printf("[server] handshake: removed wkp\n");

    //connect to client, send message
    *to_client = open(buffer, O_WRONLY, 0);
    write(*to_client, buffer, sizeof(buffer));

    //read for client
    read(from_client, buffer, sizeof(buffer));
    printf("[server] handshake received: %s\n", buffer);

    return from_client;
}

/*=========================
  client_handshake
args: int * to_server

Performs the client side pipe 3 way handshake.
Sets *to_server to the file descriptor for the upstream pipe.

returns the file descriptor for the downstream pipe.
=========================*/
int client_handshake(int *to_server) {

    int from_server;
    char buffer[HANDSHAKE_BUFFER_SIZE];

    //send pp name to server
    printf("[client] handshake: connecting to wkp\n");
    *to_server = open( WELL_KNOWN_PIPE_NAME, O_WRONLY, 0);
    if ( *to_server == -1 )
        exit(1);

    //make private pipe
    sprintf(buffer, "%d", getpid() );
    mkfifo(buffer, 0600);

    write(*to_server, buffer, sizeof(buffer));

    //open and wait for connection
    from_server = open(buffer, O_RDONLY, 0);
    read(from_server, buffer, sizeof(buffer));
    /*validate buffer code goes here */
    printf("[client] handshake: received [%s]\n", buffer);

    //remove pp
    remove(buffer);
    printf("[client] handshake: removed pp\n");

    //send ACK to server
    write(*to_server, ACK, sizeof(buffer));

    return from_server;
}
