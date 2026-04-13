/* Server code in C */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>


std::string sizeString(int s){ // 293
    std::string num = "000";

    if(s >= 100){ // 293
        num[0] = s/100 + '0'; // 2
        s %= 100; // 93
    }
    if(s >= 10){ // 93
        num[1] = s/10 + '0'; // 9
        s %= 10; // 3
    } 
    if( s >= 1){ 
        num[2] = s + '0'; // 3
    }
    return num;
}


int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));

  listen(SocketFD, 10);

  printf("-------------------------SERVER-------------------------\n");

  std::string nickname;
  std::string msg; 
  
  std::cout << "nickname: ";
  std::getline(std::cin, nickname);
  
  std::string sizeNick = sizeString(nickname.length());

  // connect -> client , socket -> server
  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    for(;;)
    {
 
        // READ
        char buffer[256];
        int n = read(ConnectFD, buffer, 3);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        std::string nick2 = buffer;
        
        n = read(ConnectFD, buffer, 3);
        buffer[n] = '\0';

        int ll = atoi(buffer);
        n = read(ConnectFD, buffer, ll);
        buffer[n] = '\0';

        std::string msg2 = buffer;
        
        std::cout << "'" << nick2 << "': " << msg2 << std::endl;  
        
        // WRITE
        std::cout << "message: ";
        std::getline(std::cin, msg);

        std::string sizeMsg = sizeString(msg.length());

        std::string nose = sizeNick + nickname + sizeMsg + msg;

        write(ConnectFD, nose.c_str(), nose.length());
      
    }

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}
