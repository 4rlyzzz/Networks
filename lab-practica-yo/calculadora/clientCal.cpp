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
/*
void diego(int ConnectFD){
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
    
    std::cout << "'" << nick2 << "': " << msg2 << std::endl; 
  }
}
*/
int main(void)
{
struct sockaddr_in stSockAddr;
int Res;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
int n;

memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

stSockAddr.sin_family = AF_INET;
stSockAddr.sin_port = htons(45000);
Res = inet_pton(AF_INET, "192.168.1.12", &stSockAddr.sin_addr);

if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
{
  perror("connect failed");
  close(SocketFD);
  exit(EXIT_FAILURE);
}


printf("-------------------------CLIENT-------------------------\n");

/*
std::string nickname; 
std::string msg; 

std::cout << "nickname: ";
std::getline(std::cin, nickname);
std::cout << "\n";

std::string sizeNick = sizeString(nickname.length());

std::thread t1 (diego, SocketFD);
*/

std::string num2, num1, op;

for(;;){

  // WRITE
  std::cout << "num1 num2 op: ";
  std::cin>> num1 >> num2 >> op;

  std::string nose = sizeString(num1.length()) + num1 + sizeString(num2.length()) + num2 + sizeString(op.length()) + op;

  write(SocketFD, nose.c_str(), nose.length());
  
  
  //READ
  //op
  char buffer[256];
  int n = read(SocketFD, buffer, 3);
  buffer[n] = '\0';

  int l = atoi(buffer);
  n = read(SocketFD, buffer, l);
  buffer[n] = '\0';

  std::string opRes = buffer;
  
  //res
  n = read(SocketFD, buffer, 3);
  buffer[n] = '\0';

  int ll = atoi(buffer);
  n = read(SocketFD, buffer, ll);
  buffer[n] = '\0';

  std::string res = buffer;
  
  std::cout << opRes << ": " << res << std::endl; 

  
  
}

shutdown(SocketFD, SHUT_RDWR);
close(SocketFD);
return 0;	
}
