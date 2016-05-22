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
#include "Aufgabe2.h"

int main(int argc, char **argv) {
	const int BUFFERSIZE = 1500;
    char buffer[BUFFERSIZE] = {0};
    char temp[1500] = "./received/";
    int sd, clientLen, i, expectedSeq;
    unsigned short port, fileNameSize;
    unsigned int fileSize, currentSeq;
    FILE *fp;
    unsigned char hash[SHA_DIGEST_LENGTH];
    static char *sha1string;

    struct sockaddr_in servAddr, clientAddr;
    socklen_t addr_size, client_addr_size;

    if (argc != 2) {
    	//error
    	return 0;
    }
    
    port = atoi(argv[1]);

    if (port < 1) {
    	printf("%s", port_error, argv[1]);
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
    
    if (recvfrom(sd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
    	printf("Failed to receive data.");
    	return 0;
    }

    if (buffer[0] != HEADER_T) {
    	printf(packet_error);
    	return 0;
    }
    
    memcpy(&fileNameSize, &buffer[1], 2);
    fileNameSize = ntohs(fileNameSize);
    
    memcpy(&fileSize, &buffer[fileNameSize + 3], 4);
    fileSize = ntohl(fileSize);

    for (i = 0; i < fileNameSize; i++)
    	temp[i + 11] = buffer[i + 3];

	memset(buffer, 0, BUFFERSIZE);
    remove(temp);
    fp = fopen(temp, "ab");
    i = 0;
    expectedSeq = 0;

    while (i < fileSize) {

        if (recvfrom(sd, buffer, BUFFERSIZE, 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
        	printf("Failed to receive data.");
        	return 0;
        }

        if (buffer[0] != DATA_T) {
        	printf(packet_error);
        	return 0;
        }

        memcpy(&currentSeq, &buffer[1], 4);
        currentSeq = ntohl(currentSeq);

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
    	printf("Failed to receive data.");
    	return 0;
    }

    if (buffer[0] != SHA1_T) {
    	printf(packet_error);
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

    if (sendto(sd, buffer, sizeof(buffer), 0, (struct sockaddr *)&clientAddr, &clientLen) < 0) {
    	//error
    	return 0;
    }

    return 0;
}
