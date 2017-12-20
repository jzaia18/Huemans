#include "pipe_networking.h"

int main() {

    int to_server;
    int from_server;
    char buffer[BUFFER_SIZE];

    printf("[client] START\n");
    from_server = client_handshake( &to_server );
    printf("[client] FINISH\n");

    exit(0);

    while (1) {
        printf("enter data: ");
        fgets(buffer, sizeof(buffer), stdin);
        *strchr(buffer, '\n') = 0;
        write(to_server, buffer, sizeof(buffer));
        read(from_server, buffer, sizeof(buffer));
        printf("received: [%s]\n", buffer);
    }
}
