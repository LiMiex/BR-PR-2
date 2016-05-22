//
// Created by Marco on 19/05/2016.
//

#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <openssl/sha.h>
#include <time.h>
#include "Aufgabe2.h"
#define BUFFERSIZE 1500

int main(int argc, char **argv) {
    char buffer[BUFFERSIZE];
    char temp[1500] = "./received/";
    int sd, i, expectedSeq;
    unsigned short port, fileNameSize;
    unsigned int fileSize, currentSeq, clientLen;
    FILE *fp;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static char *sha1string;
    struct timeval timer;
    struct sockaddr_in servAddr, clientAddr;

    timer.tv_sec = 10;
    timer.tv_usec = 0;

    if (argc != 2) {
    	//error
    	return 0;
    }

    port = atoi(argv[1]);

    if (port < 1) {
    	printf(port_error, argv[1]);
    	return 0;
    }

    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    	//error
    	return 0;
    }

    memset(&servAddr, 0, sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = INADDR_ANY;
    servAddr.sin_port = htons(port);

    if ((bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr))) == -1) {
    	//error
    	return 0;
    }

    clientLen = sizeof(clientAddr);
	memset(buffer, 0, BUFFERSIZE);

	printf("\nAbout to receive\n");

    if (recvfrom(sd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
    	printf("Failed to receive data.");
    	return 0;
    }

    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &timer, sizeof(timer)) < 0) {
        perror("Error");
    }

    if (buffer[0] != HEADER_T) {
    	printf(packet_error);
    	return 0;
    }
	printf("%d", strlen(buffer));

    memcpy(&fileNameSize, &buffer[1], 2);
    fileNameSize = ntohs(fileNameSize);

    memcpy(&fileSize, &buffer[fileNameSize + 3], 4);
    fileSize = ntohl(fileSize);

    printf("\n%d %d\n", fileSize, fileNameSize);

    for (i = 0; i < fileNameSize; i++)
    	temp[i + 11] = buffer[i + 3];

    printf("not here");

	memset(buffer, 0, BUFFERSIZE);
    remove(temp);
    fp = fopen(temp, "ab");
    i = 0;
    expectedSeq = 0;

    printf("\nHeader received\n");
    printf("%d", fileNameSize);

    while (i < fileSize) {

        if (recvfrom(sd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
        	printf(timeout_error);
        	return 0;
        }

        printf("a");

        if (buffer[0] != DATA_T) {
        	printf("\n DATA_T \n");
        	printf(packet_error);
        	return 0;
        }

        printf("b");

        memcpy(&currentSeq, &buffer[1], 4);
        currentSeq = ntohl(currentSeq);

        printf("c");

        if (currentSeq != expectedSeq) {
        	printf(order_error, currentSeq, expectedSeq);
        	return 0;
        }

        fwrite(&buffer[5], 1, strlen(&buffer[5]), fp);
        i += strlen(&buffer[5]);
        expectedSeq++;
    	memset(buffer, 0, BUFFERSIZE);
    }

    if (recvfrom(sd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
    	printf(timeout_error);
    	return 0;
    }

    if (buffer[0] != SHA1_T) {
    	printf(packet_error);
    	printf("Schleife?\n");
    	return 0;
    }

    rewind(fp);
    char *string = malloc(fileSize + 1);

    if (fread(string, 1, fileSize, fp) != fileSize) {
    	//error
    	return 0;
    }

    fclose(fp);
    SHA1(string, strlen(string), hash);
    sha1string = create_sha1_string(hash);

    if (!strcmp(sha1string, &buffer[1])) {
    	memset(buffer, 0, BUFFERSIZE);
    	memcpy(buffer, &SHA1_CMP_T, 1);
    	memcpy(&buffer[1], &SHA1_OK, 1);
    } else {
    	memset(buffer, 0, BUFFERSIZE);
    	memcpy(buffer, &SHA1_CMP_T, 1);
    	memcpy(&buffer[1], &SHA1_ERROR, 1);
    }

    free(string);

    if (sendto(sd, buffer, 2, 0, (struct sockaddr *)&clientAddr, sizeof(struct sockaddr_in)) < 0) {
    	//error
    	return 0;
    }

    return 0;
}
