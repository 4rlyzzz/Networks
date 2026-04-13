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

void cal(int ConnectFD){
  char buffer[256];
  while(true){
    // READ
    //v1
    int n = read(ConnectFD, buffer, 3);
    buffer[n] = '\0';

    int l = atoi(buffer);
    n = read(ConnectFD, buffer, l);
    buffer[n] = '\0';

    double v1 = atof(buffer);
    
    //v2
    n = read(ConnectFD, buffer, 3);
    buffer[n] = '\0';

    int l2 = atoi(buffer);
    n = read(ConnectFD, buffer, l2);
    buffer[n] = '\0';

    double v2 = atof(buffer);
    
    // op
    n = read(ConnectFD, buffer, 3);
    buffer[n] = '\0';

    int ll = atoi(buffer);
    n = read(ConnectFD, buffer, ll);
    buffer[n] = '\0';

    std::string op = buffer;
    
    // resolve
    std::string tipoOp = "";
    double res = 0;
    if (op == "+") { 
      res = v1 + v2; 
      tipoOp = "La suma"; 
    }
    else if (op == "-") { 
      res = v1 - v2; 
      tipoOp = "La resta"; 
    }
    else if (op == "*") { 
      res = v1 * v2; 
      tipoOp = "La multiplicacion"; 
    }
    else if (op == "/") { 
      res =  v1 / v2; 
      tipoOp = "La division"; 
    }
    
    std::string resStr = std::to_string(res);
    
    std::string resFinal = sizeString(tipoOp.length()) + tipoOp + sizeString(resStr.length()) + resStr;
    
    write(ConnectFD, resFinal.c_str(), resFinal.length());
    
  }
  
  shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD);
}

int main(void)
{
  struct sockaddr_in stSockAddr;
  int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  char buffer[256];
  int n;

  memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

  stSockAddr.sin_family = AF_INET;
  stSockAddr.sin_port = htons(45000);
  stSockAddr.sin_addr.s_addr = INADDR_ANY;

  bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in));

  listen(SocketFD, 10);

  printf("-------------------------SERVER-------------------------\n");
/*
  std::string nickname;
  std::string msg; 
  
  std::cout << "nickname: ";
  std::getline(std::cin, nickname);
  std::cout << "\n";
  
  std::string sizeNick = sizeString(nickname.length());

*/

  // connect -> client , socket -> server
  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    std::thread t1 (cal, ConnectFD);


    t1.join();
    
  }

  close(SocketFD);
  return 0;
}
