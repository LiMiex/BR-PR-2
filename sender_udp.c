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
#include "file.c"




int main (int argc, char *argv[]) {
    char buffer[256];
    struct sockaddr_in server;
    int sock, send;
    long fileSize;
    
    FILE *fp = fopen(argv[3], "a");
    if (fp == NULL) {
        puts("error in file handling");
        return 1;
    }
    
    fileSize = fileLength(fp);
    
    if (argc != 4){
        printf("arguments invalid");
    //creating udp socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("cannot create socket");
        return 0;
    }
    //server structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    
    if((sendto(sock, )) < 0){
        printf("cannot sendto server");
    }
}