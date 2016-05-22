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
#include <libgen.h>
#include <openssl/sha.h>
#include "Aufgabe2.h"
#define BUFFERSIZE 1492



int main (int argc, char *argv[]) {
    char buffer[BUFFERSIZE];
    //bzero(buffer,BUFFERSIZE);
    struct sockaddr_in dest, from;
    int sock, send, recv, clo;
    unsigned short filenameLength;
    long fileSize;
    unsigned int current, times;
    char *filename;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static char *sha1string;
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
        exit(1);
    }
    
    //informations for the header
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    filename = basename(argv[3]);
    filenameLength = sizeof(filename) / sizeof(char*);

    //setting buffer with null terminals
    memset(buffer,0,BUFFERSIZE);
    memmove(&buffer[0],&HEADER_T,1);
    memmove(&(buffer[1]),&filenameLength,2);
    memmove(&buffer[3],&filename,filenameLength);
    memmove(&buffer[3+filenameLength],(unsigned int) fileSize,4);
    

    //server structure
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(inet_addr(argv[1]));
    dest.sin_port = htons(atoi(argv[2]));
    
    //sending header
    if((send = sendto(sock,buffer,strlen(buffer)+1,0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
        printf("cannot sendto server");
    }
    
    //part 2
    while(current != (unsigned int) fileSize){
        memset(buffer,0,BUFFERSIZE);
        memmove(&buffer[0],&DATA_T,1);
        memmove(&buffer[1],&times,4);
        //5 for data_t and times, 1 for \0
        if(((unsigned int) fileSize - current) > (BUFFERSIZE - 6)){
            fread(&buffer[5], BUFFERSIZE - 6, 1, fp);
            current = (unsigned int) ftell(fp);
        }else{
            fread(&buffer[5], (unsigned int) fileSize, 1, fp);
        }
        times++;
        if((send = sendto(sock,buffer,sizeof(buffer),0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
            printf("cannot sendto server");
        }
    }
    
    rewind(fp);
    char *string = malloc(fileSize + 1);
    fread(string, fileSize, 1, fp);
    fclose(fp);
    memset(buffer,0,BUFFERSIZE);
    
    //3. sha1
    SHA1(string, strlen(string), hash);
    sha1string = create_sha1_string(hash);
    memmove(&buffer[0],&SHA1_T,1);
    memmove(&buffer[1],sha1string,20);
    if((send = sendto(sock,buffer,sizeof(buffer),0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
        printf("cannot sendto server");
    }
    
    //4.
    memset(buffer,0,BUFFERSIZE);
    if((recv = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*) &from, sizeof(struct sockaddr_in)) < 0)){
        printf("cannot recv from server");
    }
    //printf(buffer);
    if((clo = close(sock)) < 0){
        printf("cannot close socket.");
    }
}





