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
#include "Aufgabe2.h"




int main (int argc, char *argv[]) {
    char buffer[1491];
    struct sockaddr_in server;
    int sock, send;
    unsigned short filenameLength;
    unsigned int fileSize;
    char* filename;

    
    FILE *fp = fopen(argv[3], "a");
    if (fp == NULL) {
        puts("error in file handling");
        return 1;
    }
    
    //informations for the header
    fileSize = fileLength(fp);
    filename = getFilename(argv[3]);
    filenameLength = getFilenameLength(filename);


    
    if (argc != 4){
        printf("arguments invalid");
    //creating udp socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("cannot create socket");
        return 0;
    }
    //server structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    
    if((sendto(sock, )) < 0){
        printf("cannot sendto server");
    }
}