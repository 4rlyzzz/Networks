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

#include <string>
#include <thread>

std::string sizeString(int num){
  std::string n ="000";
  if(num>=100){
    n[0] = '0'+ num/100;
    num%=100;
  }
  if(num>=10){
    n[1] = '0'+ num/10;
    num%=10;
  }
  if(num>=1){
    n[2] = '0'+ num/1;
  }
  return n;
}

void funRead(int ConnectFD){
  while(true){
    //READ
    char buffer[256];
    int n = read(ConnectFD,buffer,3);
    buffer[n] ='\0';
    
    int l =atoi(buffer);
    n = read(ConnectFD,buffer,l);
    buffer[n] ='\0';
    
    std::string n1 = buffer;
    
    n = read(ConnectFD,buffer,3);
    buffer[n] ='\0';
    
    int ll =atoi(buffer);
    n = read(ConnectFD,buffer,ll);
    buffer[n] ='\0';
    
    std::string n2 = buffer;
    
    std::cout<<"["<<n1<<"]: " << n2 << "\n";
  }
}

void funWrite(int ConnectFD, std::string sizeN, std::string nickname){
    std::string msg;
    while(true) {
        std::getline(std::cin, msg);
        
        std::string sizeM = sizeString(msg.length());
        std::string send = sizeN + nickname + sizeM + msg;
        write(ConnectFD, send.c_str(), send.length());
        
    }
    
    shutdown(ConnectFD, SHUT_RDWR);
    close(ConnectFD);
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
  
  char buffer[256];
  
  std::string nickname;
  std::cout << "nick: ";
  std::getline(std::cin, nickname);
  std::cout <<"\n";
  
  std::string sizeN = sizeString(nickname.length());
  
  

  // connect -> client , socket -> server
  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);
    
    std::thread t(funRead, ConnectFD);
    std::thread t2(funWrite, ConnectFD, sizeN, nickname);


    t.join();
    t2.join();

  }

  close(SocketFD);
  return 0;
}
