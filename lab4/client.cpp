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
  LOGIN = 1, LOGOUT, BROADCAST, UNICAST, LIST, SALIR
};

void showMenu() {
    std::cout << "\n=== Menu ===\n";
    if (!logg) {
        std::cout << "1. Login\n";
        std::cout << "6. Salir\n";
    } else {
        std::cout << "2. Logout\n";
        std::cout << "3. Broadcast msg\n";
        std::cout << "4. Unicast msg\n";
        std::cout << "5. Ver Usuarios\n";
        std::cout << "6. Salir\n";
    }
    std::cout << "Opcion: ";
}

void ejecutarComando(Menu opt, int SocketFD) {
    switch (opt) {

        case LOGIN: {
            std::string msg = "L" + sizeString(nickname.length(), 4) + nickname;
            write(SocketFD, msg.c_str(), msg.length());
            
            sleep(1);         
            break;
        }
        
        case LOGOUT:{
            write(SocketFD, "O", 1);
            logg = false;
            break;
        }

        case BROADCAST: {

            std::string msg; 
    
            std::cout << ">> Message: ";
            std::getline(std::cin, msg);
    
            std::string nose = "B" + sizeString(msg.length(), 7) + msg;
    
            write(SocketFD, nose.c_str(), nose.length());

            break;
        }

        case UNICAST:{

            std::string dest;
            std::string msg; 
    
            std::cout << ">> Destinatary: ";
            std::getline(std::cin, dest);
            std::cout << "\n";
            
            std::cout << ">> Message: ";
            std::getline(std::cin, msg);
    
            std::string nose = "U" + sizeString(msg.length(), 5) + msg + sizeString(dest.length(), 7) + dest;
    
            write(SocketFD, nose.c_str(), nose.length());
    
            break;
        }

        case LIST: {
            break;
        }

        case SALIR:
            std::cout << ">> Saliendo del programa..." << std::endl;
            break;

        default:
            std::cout << ">> Opcion invalida." << std::endl;
            break;
    }
}




void readThread(int ConnectFD){
  // READ
  while(true){
    char buffer[256];
    int n = read(ConnectFD, buffer, 1);
    buffer[n] = '\0';

    if(buffer[0] == 'l'){ // login - protocolo: L + 4B size + Nick
        n = read(ConnectFD, buffer, 4);
        buffer[n] = '\0';
        
        int l = atoi(buffer);
        n = read(ConnectFD, buffer, 3);
        buffer[n] = '\0';

        std::string nick2 = buffer;
    }

    else if(buffer[0] == 'k'){
        logg = true;

        std::cout << ">> Login exitoso" << std::endl;
    }

    else if(buffer[0] == 'e'){
        n = read(ConnectFD, buffer, 5);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        logg = false;

        std::cout << ">> Error: " << buffer << std::endl;

        if(!logg){ 
            std::cout << "Ingresa otro nickname: ";
            std::getline(std::cin, nickname);
        }
    }

    else if(buffer[0] == 'b'){ // broadcast

        n = read(ConnectFD, buffer, 3);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        std::string msg2 = buffer;

        n = read(ConnectFD, buffer, 7);
        buffer[n] = '\0';

        int ll = atoi(buffer);
        n = read(ConnectFD, buffer, ll);
        buffer[n] = '\0';

        std::string nickDest = buffer;
        
        std::cout << "[" << nickDest << "]: " << msg2 << std::endl;

    }
    
    else if(buffer[0] == 'u'){ // unicast - protocolo U + 5B sizeMsg + msg + 7B sizeDest + dest

        n = read(ConnectFD, buffer, 7);
        buffer[n] = '\0';

        int l = atoi(buffer);
        n = read(ConnectFD, buffer, l);
        buffer[n] = '\0';

        std::string nickDest = buffer;

        n = read(ConnectFD, buffer, 5);
        buffer[n] = '\0';

        int ll = atoi(buffer);
        n = read(ConnectFD, buffer, ll);
        buffer[n] = '\0';

        std::string msg2 = buffer;
        
        std::cout << "[" << nickDest << "]: " << msg2 << std::endl;

    }

    else if(buffer[0] == 't'){ // lista usuarios
    }

    else if(buffer[0] == 'f'){ // file
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

    std::cout << "nickname: ";
    std::getline(std::cin, nickname);
    std::cout << "\n";

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
        showMenu();

    } while(opt != SALIR);





    //t1.join();
    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;	
}