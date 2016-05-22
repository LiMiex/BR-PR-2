//
//  sender_udp.c
//  Aufgabe2_UDP
//
//  Created by Andy Shek on 20.05.16.
//  Copyright © 2016 Andy Shek. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>
#include <openssl/sha.h>
#include <errno.h>
#include "Aufgabe2.h"

#define BUFFERSIZE 1492



int main (int argc, char *argv[]) {
    char buffer[BUFFERSIZE];
    //bzero(buffer,BUFFERSIZE);
    struct sockaddr_in dest, from;
    socklen_t addr_size, client_addr_size;
    int counter;

    int sock, send, recv, clo, flen;
    unsigned short filenameLength;
    long fileSize;
    unsigned int current, times, uifileSize;
    char *filename;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static char *sha1string;
    
    flen = sizeof(struct sockaddr_in);
    current = 0;
    times = 0;

    if (argc != 4){
        printf("arguments invalid");
        return 0;
    }
    
    //creating udp socket
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("cannot create socket");
        return 0;
    }

    FILE *fp = fopen(argv[3], "r");
    if (fp == NULL) {
        puts("error in file handling");
        return 0;
    }
    
    //informations for the header
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    uifileSize = (unsigned int) fileSize;
    filename = basename(argv[3]);
    filenameLength = strlen(filename);

    //setting buffer with null terminals
    memset(buffer,0,BUFFERSIZE);
    memmove(&buffer[0],&HEADER_T,1);
    memmove(&(buffer[1]),&filenameLength,2);
    memmove(&buffer[3],&filename,filenameLength);
    memmove(&buffer[3+filenameLength],&uifileSize,4);
    
    //server structure
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = inet_addr(argv[1]);
    dest.sin_port = htons(atoi(argv[2]));
    //sending header
    usleep(100000);
    if((sendto(sock,buffer,7+filenameLength,0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
        printf("errno: %d\n\n", errno);
        printf("%d", sock);
        return 0;
    }
    
    //part 2
    while(current != (unsigned int) fileSize){
        memset(buffer,0,BUFFERSIZE);
        memmove(&buffer[0],&DATA_T,1);
        memmove(&buffer[1],&times,4);
        //5 for data_t and times, 1 for \0
        if(((unsigned int) fileSize - current) > (BUFFERSIZE - 6)){
            fread(&buffer[5], BUFFERSIZE - 6, 1, fp);
            current += (unsigned int) ftell(fp);
            if((send = sendto(sock,buffer,BUFFERSIZE,0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
                printf("cannot sendto server%d", times);
                return 0;
            }
        }else{
            fread(&buffer[5], (unsigned int) fileSize, 1, fp);
            if((send = sendto(sock,buffer,(unsigned int) fileSize - current + 6,0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
                printf("cannot sendto server%d", times);
                return 0;
            }
        }
        times++;
        usleep(100000);
        //printf("%s", buffer);

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
    usleep(100000);
    if((send = sendto(sock,buffer,21,0,(struct sockaddr*) &dest,sizeof(struct sockaddr_in) )) < 0){
        printf("cannot sendto server");
        return 0;
    }
    
    //4.
    memset(buffer,0,BUFFERSIZE);
    if((recv = recvfrom(sock, buffer, BUFFERSIZE, 0, (struct sockaddr*) &from,&flen)) < 0){
        printf("cannot recv from server");
        return 0;
    }
    //printf(buffer);
    if((clo = close(sock)) < 0){
        printf("cannot close socket.");
        return 0;
    }
}
