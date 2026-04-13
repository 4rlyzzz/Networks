/* Server code in C */
/*
PROTOCOL C->S
LOGIN:     L + 4B(size_nick) + nick
LOGOUT:    O
BROADCAST: B + 7B(size_msg) + msg
UNICAST:   U + 5B(size_msg) + msg + 7B(size_dest) + dest
LIST:      T
FILE:      F

PROTOCOL S->C
LOGIN OK:  k
ERROR:     e + 5B(size_msg) + msg
BROADCAST: b + 3B(size_msg) + msg + 7B(size_orig) + orig
UNICAST:   u + 7B(size_orig) + orig + 5B(size_msg) + msg
LIST:      t + 5B(size_data) + data
FILE:      f
*/

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


std::string sizeString(int s, int digitos) {
    std::string num = "";

    for(int i = 0; i < digitos; i++) 
    num += "0";

    for(int i = digitos - 1; i >= 0; i--) {
        num[i] = (s % 10) + '0';
        s /= 10;
    }
    return num;
}

void sendOk(int fd){
    write(fd, "k", 1);
}

void sendError(int fd, std::string msg){
    std::string send = "e" + sizeString(msg.length(), 5) + msg;
    write(fd, send.c_str(), send.length());
}

// receives commands from the client (C->S) and sends responses (S->C)
void readThread(int ConnectFD){
  while(true){
    // READ
    char buffer[256];
    int n = read(ConnectFD, buffer, 1);
    buffer[n] = '\0';

    // LOGIN - PROTOCOL (C->S): L + 4B(size_nick) + nick 
    //                  (S->C): k / e + 5B(size_msg) + msg
    if(buffer[0] == 'L'){ 
      n = read(ConnectFD, buffer, 4);
      buffer[n] = '\0';
      
      int l = atoi(buffer);
      n = read(ConnectFD, buffer, l);
      buffer[n] = '\0';

      std::string nick2 = buffer;

      /////////////////////////////

      // Check
      if(buff.find(nick2) != buff.end()){
        sendError(ConnectFD, "Nick in use");
      } 
      else {
        buff[nick2] = ConnectFD;
        sendOk(ConnectFD);
      }
    }

    // LOGOUT - PROTOCOL (C->S): O 
    //                   (S->C): k
    else if(buffer[0] == 'O'){
      for(auto& i: buff){
        if(i.second == ConnectFD){
          buff.erase(i.first);
          sendOk(ConnectFD);
          break;
        }
      }
    }

    // BROADCAST - PROTOCOL (C->S): B + 7B(size_msg) + msg 
    //                      (S->C): b + 3B(size_msg) + msg + 7B(size_orig) + orig
    else if(buffer[0] == 'B'){
      n = read(ConnectFD, buffer, 7);
      buffer[n] = '\0';

      int l = atoi(buffer);
      n = read(ConnectFD, buffer, l);
      buffer[n] = '\0';

      std::string msg = buffer;

      /////////////////////////////

      std::string orig;
      for(auto i: buff){
        if(i.second == ConnectFD){
          orig = i.first;
          break;
        }
      }

      std::string send = "b" + sizeString(msg.length(), 3) + msg +sizeString(orig.length(), 7) + orig;
      
      for(auto i: buff)
        write(i.second, send.c_str(), send.length());
      
    }

    // UNICAST - PROTOCOL (C->S): U + 5B(size_msg) + msg + 7B(size_dest) + dest 
    //                    (S->C) u + 7B(size_orig) + orig + 5B(size_msg) + msg
    else if(buffer[0] == 'U'){ 

      n = read(ConnectFD, buffer, 5);
      buffer[n] = '\0';

      int l = atoi(buffer);
      n = read(ConnectFD, buffer, l);
      buffer[n] = '\0';

      std::string msg = buffer;

      n = read(ConnectFD, buffer, 7);
        buffer[n] = '\0';

      int ll = atoi(buffer);
      n = read(ConnectFD, buffer, ll);
      buffer[n] = '\0';

      std::string nick= buffer;

      /////////////////////////////

      std::string orig;
      for(auto i: buff){
        if(i.second == ConnectFD){
          orig = i.first;
          break;
        }
      }

      std::string send = "u" + sizeString(orig.length(), 7) + orig + sizeString(msg.length(), 5) + msg;

      // Check
      if(buff.find(nick) != buff.end())
        write(buff[nick], send.c_str(), send.length());
      else
        sendError(ConnectFD, "User not found");
    }

    // LIST - PROTOCOL(C->S): T 
    //                (S->C): t + 5B(size_data) + data
    else if(buffer[0] == 'T'){ 
    }

    // FILE - PROTOCOL (C->S): F
    else if(buffer[0] == 'F'){
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
  
  if(bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)) < 0) {
    perror("error bind failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  listen(SocketFD, 10);
  

  printf("-------------------------SERVER-------------------------\n");
  

  char buff_name[256];


  // connect -> client , socket -> server
  for(;;)
  {
    int ConnectFD = accept(SocketFD, NULL, NULL);

    std::thread t1 (readThread, ConnectFD);
    t1.detach();
    
  }
  close(SocketFD);
  return 0;
}