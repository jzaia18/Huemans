#include "pipe_networking.h"
#include <signal.h>

void process(char *s);
void subserver(int from_client);

static void sighandler(int signo) {
    if (signo == SIGINT) {
        remove(WELL_KNOWN_PIPE_NAME);
        exit(0);
    }
}

int main() {
}

void subserver(int from_client) {
}

void process(char * s) {
}
