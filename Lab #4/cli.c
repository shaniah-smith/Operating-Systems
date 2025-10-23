#include <netinet/in.h> // structure for storing address information
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h> // for socket APIs
#include <sys/types.h>
#include <unistd.h>

#define PORT 9001
#define MAX_COMMAND_LINE_LEN 1024

char* getCommandLine(char *command_line){
  do {
    if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
      fprintf(stderr, "fgets error");
      exit(1);
    }
  } while (command_line[0] == 0x0A); // while just ENTER pressed
  command_line[strlen(command_line) - 1] = '\0';
  return command_line;
}

int main(int argc, char const* argv[]) {
  int sockID = socket(AF_INET, SOCK_STREAM, 0);
  if (sockID < 0) { perror("socket"); return 1; }

  char  *token, *cp;
  char buf[MAX_COMMAND_LINE_LEN];
  char responeData[MAX_COMMAND_LINE_LEN];

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(PORT);
  servAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1

  int connectStatus = connect(sockID, (struct sockaddr*)&servAddr, sizeof(servAddr));
  if (connectStatus == -1) {
    perror("connect");
    close(sockID);
    return 1;
  }

  while (1) {
    printf("Enter Command (or menu): ");
    getCommandLine(buf);

    send(sockID, buf, strlen(buf), 0);  // send command and args to server

    cp = buf;
    token = strtok(cp, " ");

    if (token && strcmp(token,"exit") == 0){
      // server will respond and close; we'll read once and break
    }
    else if (token && strcmp(token,"menu") == 0){
      printf("COMMANDS:\n---------\n"
             "1. print\n2. get_length\n3. add_back <value>\n"
             "4. add_front <value>\n5. add_position <index> <value>\n"
             "6. remove_back\n7. remove_front\n8. remove_position <index>\n"
             "9. get <index>\n10. exit\n");
    }

    int m = recv(sockID, responeData, sizeof(responeData)-1, 0);
    if (m <= 0) { printf("\nSERVER closed the connection.\n"); break; }
    responeData[m] = '\0';

    printf("\nSERVER RESPONSE: %s\n", responeData);

    if (token && strcmp(token,"exit") == 0) break;
    memset(buf, '\0', MAX_COMMAND_LINE_LEN);
  }

  close(sockID);
  return 0;
}
