/* Client code in C */
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
#include <string>
#include <thread>
#include <map>

std::map<std::string, int> buff;
std::string nickname;
bool logg = false;



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

enum Menu {
  LOGIN = 1, LOGOUT, BROADCAST, UNICAST, LIST, SEND_FILE, SALIR
};

void showMenu() {
    std::cout << "\n=== Menu ===\n";
    if (!logg) {
        std::cout << "1. Login\n";
        std::cout << "7. Salir\n";
    } else {
        std::cout << "2. Logout\n";
        std::cout << "3. Broadcast\n";
        std::cout << "4. Unicast\n";
        std::cout << "5. View users\n";
        std::cout << "6. Send File\n";
        std::cout << "7. Salir\n";
    }
    std::cout << ">> Option: ";
}

// sends commands to the server (C->S)
void ejecutarComando(Menu opt, int SocketFD) {
    switch (opt) {

        // LOGIN - PROTOCOL (C->S): L + 4B(size_nick) + nick
        case LOGIN: {
            std::string msg = "L" + sizeString(nickname.length(), 4) + nickname;
            write(SocketFD, msg.c_str(), msg.length());       
            sleep(1);         
            break;
        }

        // LOGOUT - PROTOCOL (C->S): O
        case LOGOUT:{
            write(SocketFD, "O", 1);
            logg = false;
            break;
        }

        // BROADCAST - PROTOCOL (C->S): B + 7B(size_msg) + msg
        case BROADCAST: {

            std::string msg; 
    
            std::cout << ">> Message: ";
            std::getline(std::cin, msg);
    
            std::string nose = "B" + sizeString(msg.length(), 7) + msg;
    
            write(SocketFD, nose.c_str(), nose.length());

            break;
        }

        // UNICAST - PROTOCOL (C->S): U + 5B(size_msg) + msg + 7B(size_dest) + dest
        case UNICAST:{

            std::string dest;
            std::string msg; 
    
            std::cout << ">> Addressee: ";
            std::getline(std::cin, dest);
            
            std::cout << ">> Message: ";
            std::getline(std::cin, msg);
    
            std::string nose = "U" + sizeString(msg.length(), 5) + msg + sizeString(dest.length(), 7) + dest;
    
            write(SocketFD, nose.c_str(), nose.length());
    
            break;
        }

        // LIST - PROTOCOL (C->S): T
        case LIST: {
            break;
        }

        // FILE - PROTOCOL (C->S): F
        case SEND_FILE: {
            break;
        }

        case SALIR: {
            break;
        }

        default: {
            std::cout << ">> Invalid option" << std::endl;
            break;
        }
    }
}

// receives and processes messages from the server (S->C)
void readThread(int ConnectFD){
  // READ
  while(true){
    char buffer[256];
    int n = read(ConnectFD, buffer, 1);
    buffer[n] = '\0';

    // LOGIN - PROTOCOL: L + 4B(size_nick) + nick
    if(buffer[0] == 'l'){ 
        n = read(ConnectFD, buffer, 4);
        buffer[n] = '\0';
        
        int l = atoi(buffer);
        n = read(ConnectFD, buffer, 3);
        buffer[n] = '\0';

        std::string nick = buffer;
    }

    // LOGIN - PROTOCOL (S->C): k
    else if(buffer[0] == 'k'){

        logg = true;
        if(logg)
            std::cout << ">> Successful login" << std::endl;
        else
            std::cout << ">> Successful logout" << std::endl;
    }

    // ERROR - PROTOCOL (S->C): e + 5B(size_msg) + msg
    else if(buffer[0] == 'e'){
        n = read(ConnectFD, buffer, 5);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        logg = false;

        std::cout << ">> Error: " << buffer << std::endl;

        if(!logg){ 
            std::cout << ">> Enter another nickname: ";
            std::getline(std::cin, nickname);
        }
    }

    // BROADCAST - PROTOCOL (S->C): b + 3B(size_msg) + msg + 7B(size_nickOrig) + nickOrig
    else if(buffer[0] == 'b'){ 

        n = read(ConnectFD, buffer, 3);
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

        std::string nickOrig = buffer;
        
        std::cout << "[" << nickOrig << "]: " << msg << std::endl;

    }

    // UNICAST - PROTOCOL (S->C): u + 7B(size_nickOrig) + nickOrig + 5B(size_msg) + msg
    else if(buffer[0] == 'u'){ 
        n = read(ConnectFD, buffer, 7);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        std::string nickOrig = buffer;

        n = read(ConnectFD, buffer, 5);
        buffer[n] = '\0';

        int ll = atoi(buffer);
        n = read(ConnectFD, buffer, ll);
        buffer[n] = '\0';

        std::string msg = buffer;
        
        std::cout << "[" << nickOrig << "]: " << msg << std::endl;

    }

    // LIST - PROTOCOL (S->C): t + 5B(size_data) + data
    else if(buffer[0] == 't'){ 
    }

    // FILE - PROTOCOL (S->C): f
    else if(buffer[0] == 'f'){ 
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

    inet_pton(AF_INET, "192.168.1.12", &stSockAddr.sin_addr);
    
    if(connect(SocketFD, (const struct sockaddr*)&stSockAddr, sizeof(struct sockaddr_in)) < 0){
    std::cout << "connect failed" << std::endl;
    close(SocketFD);
    return 1;
}

    printf("-------------------------CLIENT-------------------------\n");

    std::cout << "Nickname: ";
    std::getline(std::cin, nickname);

    std::thread t1 (readThread, SocketFD);
    t1.detach();

    int val;
    enum Menu opt;

    showMenu();

    do{
        scanf("%d", &val);
        getchar();
        opt = static_cast<Menu>(val);

        if(logg || opt == LOGIN || opt == SALIR)
            ejecutarComando(opt, SocketFD);

        if(opt != SALIR)
            showMenu();

    } while(opt != SALIR);

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;	
}