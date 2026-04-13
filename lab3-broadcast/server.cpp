/* Server code in C */
// PROTOCOLO: 3B(size_nickname) + nickname + 3B(size_msg) + msg

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <map>

std::map<std::string, int> buff;


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

void readThread(int ConnectFD){
  while(true){
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
    
    std::string send = sizeString(nick2.length()) + nick2 + sizeString(msg2.length()) + msg2;
    
    for(auto it: buff){
      n = write(it.second, send.c_str(), send.length());
    }
    
  }
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


  // connect -> client , socket -> server
  for(;;)
  {
    char buff_name[256];
    int ConnectFD = accept(SocketFD, NULL, NULL);
    int n = read(ConnectFD, buff_name, 255);
    buff_name[n]='\0';
    std::string nickname = buff_name;
    
    buff[nickname] = ConnectFD;

    std::thread t1 (readThread, ConnectFD);
    t1.detach();
    
    //shutdown(ConnectFD, SHUT_RDWR);

    //close(ConnectFD);
  }

  close(SocketFD);
  return 0;
}
