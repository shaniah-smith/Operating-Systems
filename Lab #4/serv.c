// serv.c
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "list.h"

#define PORT 9001
#define ACK  "ACK"

int main(int argc, char const* argv[]) {
  int n, val, idx;

  int servSockD = socket(AF_INET, SOCK_STREAM, 0);
  if (servSockD < 0) { perror("socket"); return 1; }

  int opt = 1;
  setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  char buf[1024];
  char sbuf[1024];
  char *token;

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_port   = htons(PORT);
  servAddr.sin_addr.s_addr = INADDR_ANY;

  if (bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
    perror("bind"); close(servSockD); return 1;
  }
  if (listen(servSockD, 1) < 0) {
    perror("listen"); close(servSockD); return 1;
  }

  int clientSocket = accept(servSockD, NULL, NULL);
  if (clientSocket < 0) { perror("accept"); close(servSockD); return 1; }

  list_t *mylist = list_alloc();

  while (1) {
    n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
    if (n <= 0) break;                 // client closed or error
    buf[n] = '\0';
    sbuf[0] = '\0';

    token = strtok(buf, " ");
    if (!token) {
      snprintf(sbuf, sizeof(sbuf), "ERR empty command");
    }
    else if (strcmp(token, "exit") == 0) {
      list_free(mylist);
      snprintf(sbuf, sizeof(sbuf), "bye");
      send(clientSocket, sbuf, strlen(sbuf) + 1, 0);
      break;
    }
    else if (strcmp(token, "get_length") == 0) {
      val = list_length(mylist);
      snprintf(sbuf, sizeof(sbuf), "Length = %d", val);
    }
    else if (strcmp(token, "add_front") == 0) {
      token = strtok(NULL, " ");
      if (!token) snprintf(sbuf, sizeof(sbuf), "ERR missing <value>");
      else {
        val = atoi(token);
        list_add_to_front(mylist, val);
        snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "add_back") == 0) {
      token = strtok(NULL, " ");
      if (!token) snprintf(sbuf, sizeof(sbuf), "ERR missing <value>");
      else {
        val = atoi(token);
        list_add_to_back(mylist, val);
        snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "add_position") == 0) {
      char *tok_idx = strtok(NULL, " ");
      char *tok_val = strtok(NULL, " ");
      if (!tok_idx || !tok_val) {
        snprintf(sbuf, sizeof(sbuf), "ERR usage: add_position <index> <value>");
      } else {
        idx = atoi(tok_idx);
        val = atoi(tok_val);
        list_add_at_index(mylist, val, idx); // 1-based
        snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "remove_back") == 0) {
      val = list_remove_from_back(mylist);
      snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
    }
    else if (strcmp(token, "remove_front") == 0) {
      val = list_remove_from_front(mylist);
      snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
    }
    else if (strcmp(token, "remove_position") == 0) {
      token = strtok(NULL, " ");
      if (!token) snprintf(sbuf, sizeof(sbuf), "ERR missing <index>");
      else {
        idx = atoi(token);
        val = list_remove_at_index(mylist, idx);
        snprintf(sbuf, sizeof(sbuf), "%s%d", ACK, val);
      }
    }
    else if (strcmp(token, "get") == 0) {
      token = strtok(NULL, " ");
      if (!token) snprintf(sbuf, sizeof(sbuf), "ERR missing <index>");
      else {
        idx = atoi(token);
        val = list_get_elem_at(mylist, idx);
        snprintf(sbuf, sizeof(sbuf), "%d", val);
      }
    }
    else if (strcmp(token, "print") == 0) {
      char *s = listToString(mylist);      // malloc'ed
      snprintf(sbuf, sizeof(sbuf), "%s", s);
      free(s);
    }
    else {
      snprintf(sbuf, sizeof(sbuf), "ERR unknown command");
    }

    send(clientSocket, sbuf, strlen(sbuf) + 1, 0);
    memset(buf, 0, sizeof(buf));
  }

  list_free(mylist);
  close(clientSocket);
  close(servSockD);
  return 0;
}
