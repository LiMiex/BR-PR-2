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
#include <string.h>
#include "file.c"
#include "Aufgabe2.h"
#define BUFFERSIZE 1492



int main (int argc, char *argv[]) {
    char buffer[BUFFERSIZE];
    
    //bzero(buffer,BUFFERSIZE);
    struct sockaddr_in server;
    int sock, send;
    unsigned short filenameLength;
    unsigned int fileSize, current, times;
    char* filename;
    current = 0;
    times = 0;
    
    if (argc != 4){
        printf("arguments invalid");
        //creating udp socket
        if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            printf("cannot create socket");
            return 0;
        }
    }
    
    FILE *fp = fopen(argv[3], "rb");
    if (fp == NULL) {
        puts("error in file handling");
        return 1;
    }
    
    //informations for the header
    fileSize = fileLength(fp);
    fclose(fp);
    filename = getFilename(argv[3]);
    filenameLength = getFilenameLength(filename);
    //setting buffer with null terminals
    memset(buffer,0,BUFFERSIZE);
        
    memmove(buffer[0],HEADER_T,1);
    memmove(buffer[1],filenameLength,2);
    memmove(buffer[3],filename,filenameLength);
    memmove(buffer[4+filenameLength],fileSize,4);
    

    //server structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    
    //sending header
    if((send = sendto(sock,buffer,strlen(buffer)+1,0,(struct sockaddr*) &server,sizeof(struct sockaddr_in) )) < 0){
        printf("cannot sendto server");
    }
    
    //part 2
    while(current != fileSize){
        memset(buffer,0,BUFFERSIZE);
        mememove(buffer[0],DATA_T,1);
        mememove(buffer[1],times,4);
        //5 for data_t and times, 1 for \0
        if((fileSize - current) > (BUFFERSIZE - 6)){
            fread(buffer[5], BUFFERSIZE - 6, 1, fp);
            current = (unsigned int) ftell(fp);
        }else{
            fread(buffer[5], fileSize, 1, fp);
        }
        times++;
        if((send = sendto(sock,buffer,strlen(buffer)+1,0,(struct sockaddr*) &server,sizeof(struct sockaddr_in) )) < 0){
            printf("cannot sendto server");
        }
    }
    rewind(fp);
}





