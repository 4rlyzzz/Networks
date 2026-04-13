/* Server code in C */

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
  stSockAddr.sin_addr.s_addr = INADDR_ANY;
  
  bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));
  
  listen(SocketFD, 10);
  
  printf("-------------------------SERVER-------------------------\n");
  
  char buffer[256];

  // connect -> client , socket -> server
  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    for(;;)
    {
      bzero(buffer,256);
      read(ConnectFD,buffer,255);
      printf("client: %s\n",buffer);
      

      bzero(buffer, 256);
      printf("message: ");
      //scanf("%s", buffer);
      fgets(buffer,256,stdin);
      write(ConnectFD,buffer,strlen(buffer));
    }

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}
