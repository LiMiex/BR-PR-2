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


int main (int argc, char **argv) {
  int fd;
  //creating socket
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("cannot create socket");
    return 0;
  }
}