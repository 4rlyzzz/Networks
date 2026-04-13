/* Client code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(8888);
  inet_pton(AF_INET, "192.168.1.12", &stSockAddr.sin_addr);

  if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
  {
    perror("connect failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }


  printf("-------------------------CLIENT-------------------------\n");


  for(;;){
    char buffer[256];
    bzero(buffer, 256);
    printf("message: ");
    //scanf("%s", buffer);
    fgets(buffer,256,stdin);
    write(SocketFD,buffer,strlen(buffer));


    bzero(buffer, 256);
    read(SocketFD,buffer,255);
    printf("server: %s\n",buffer);

  }

  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);
  return 0;	
}
