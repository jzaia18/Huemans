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
    signal(SIGINT, sighandler);

    int from_client;
    while(1) {
        from_client = server_setup();

        // If child process
        int f = fork();
        if (!f)
            subserver(from_client);
        printf("[server] looking for next client\n");
    }
}

void subserver(int from_client) {
  printf("[subserver %d] successfully forked\n", getpid());
  int to_client = server_connect(from_client);
  char buf[BUFFER_SIZE];
  while (read(from_client, buf, sizeof(buf))) {
    printf("[subserver %d] received: %s\n", getpid(), buf);
    process(buf);
    write(to_client, buf, sizeof(buf));
  }
}


// rot13 (Credit to Piotr & Leo from whom we "borrowed" this)
void process(char * s) {
  while(*s){
    if((*s >= 'a' && *s <= 'm') || (*s >= 'A' && *s <= 'M')){
      *s += 13;
    }
    else if((*s >= 'n' && *s <= 'z')||(*s >= 'N' && *s <= 'Z')) {
      *s -= 13;
    }
    s++;
  }
}
