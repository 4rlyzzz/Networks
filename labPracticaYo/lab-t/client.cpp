/* Client code in C */

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

void funRead(int SocketFD){
  while(true){
    //READ
    char buffer[256];
    int n = read(SocketFD,buffer,3);
    buffer[n] ='\0';
    
    int l =atoi(buffer);
    n = read(SocketFD,buffer,l);
    buffer[n] ='\0';
    
    std::string n1 = buffer;
    
    n = read(SocketFD,buffer,3);
    buffer[n] ='\0';
    
    int ll =atoi(buffer);
    n = read(SocketFD,buffer,ll);
    buffer[n] ='\0';
    
    std::string n2 = buffer;
    
    std::cout<<"["<<n1<<"]: " << n2 << "\n";
  }
}

void funWrite(int SocketFD, std::string nSize, std::string nick){
  std::string msg;
  while(true){
    //WRITE
    //std::cout<<"message: ";
    std::getline(std::cin,msg);
    
    std::string mSize= sizeString(msg.length());
    
    std::string send=nSize+nick+mSize+msg;
    
    write(SocketFD,send.c_str(), send.length());
  }
  shutdown(SocketFD, SHUT_RDWR);
  close(SocketFD);
}


int main(void)
{
struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

stSockAddr.sin_family = AF_INET;
stSockAddr.sin_port = htons(45000);
inet_pton(AF_INET, "192.168.1.12", &stSockAddr.sin_addr);

if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
{
  perror("connect failed");
  close(SocketFD);
  exit(EXIT_FAILURE);
}


printf("-------------------------CLIENT-------------------------\n");


std::string nick;


std::cout << "nick: ";
std::getline(std::cin,nick);
std::string nSize=sizeString(nick.length());

std::thread t(funRead, SocketFD);
std::thread t2(funWrite, SocketFD, nSize, nick);


t.join();
t2.join();


return 0;	
}
