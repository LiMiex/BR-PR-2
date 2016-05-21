//
//  sender_udp.c
//  Aufgabe2_UDP
//
//  Created by Andy Shek on 20.05.16.
//  Copyright © 2016 Andy Shek. All rights reserved.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>




int main (int argc, char **argv) {
    char buffer[256];
    struct sockaddr_in server;
    int sock;
    
    //creating socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("cannot create socket");
        return 0;
    }
    //serverstruktur
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
}