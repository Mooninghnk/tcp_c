#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(void) {
  printf("conf local address\n");

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;       // ipv4
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;     // assing address to wildcard

  struct addrinfo *bind_adress;
  int ok = getaddrinfo(0, "8080", &hints, &bind_adress);
  if (ok == -1) {
    fprintf(stderr, "error occured\n");
    return 1;
  }
  printf("creating socket\n");
  int socket_listen = socket(bind_adress->ai_family, bind_adress->ai_socktype,
                             bind_adress->ai_protocol);

  if (socket_listen == -1) {
    fprintf(stderr, "error occured\n");

    return 1;
  }
  printf("binding socket to  adress");
  if (bind(socket_listen, bind_adress->ai_addr, bind_adress->ai_addrlen)) {
    fprintf(stderr, "error occured");
    return 1;
  }
  freeaddrinfo(bind_adress);
  printf("lisnting for inncomming connection\n");
  if (listen(socket_listen, 10) > 0) {
    fprintf(stderr, "error occured");
    return 1;
  }
  printf("waitin for connection\n");
  struct sockaddr_storage client_addr;
  socklen_t client_len = sizeof(client_addr);
  int socket_client =
      accept(socket_listen, (struct sockaddr *)&client_addr, &client_len);
  char address_buffer[100];
  getnameinfo((struct sockaddr *)&client_addr, client_len, address_buffer,
              sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
  printf("%s\n", address_buffer);

  char req[1024];
  int bytes_rec = recv(socket_client, req, 1024, 0);
  printf("%.*s", bytes_rec, req);

  const char *resp = "HTTP/1.1 200 OK\r\n"
                     "Content-Type: text/html\r\n"
                     "Content-Lenght: 70\r\n"
                     "\r\n"
                     "<html>"
                     "<head><title>Hello world</title></head>"
                     "</html>";

  int bytes_send = send(socket_client, resp, strlen(resp), 0);
}
