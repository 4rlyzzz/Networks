/* Client code in C */
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
#include <string>
#include <thread>


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
    
    std::cout << "[" << nick2 << "]: " << msg2 << std::endl; 
  }
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
std::string nickname; 
std::string msg; 

std::cout << "nickname: ";
std::getline(std::cin, nickname);
std::cout << "\n";

std::string sizeNick = sizeString(nickname.length());

std::thread t1 (readThread, SocketFD);

for(;;){

  // WRITE
  //std::cout << "message: ";
  std::getline(std::cin, msg);

  std::string sizeMsg = sizeString(msg.length());

  std::string nose = sizeNick + nickname + sizeMsg + msg;

  write(SocketFD, nose.c_str(), nose.length());
  
}
t1.join();
shutdown(SocketFD, SHUT_RDWR);
close(SocketFD);
return 0;	
}
