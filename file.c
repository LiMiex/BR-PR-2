//
//  file.c
//  Aufgabe2_UDP
//
//  Created by Andy Shek on 20.05.16.
//  Copyright © 2016 Andy Shek. All rights reserved.
//


#include <stdio.h>
#include <libgen.h>

// Gets the file length
unsigned int fileLength(FILE *fp){
    long size = 0;
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return (unsigned int)size;
}

char *getFilename(char *name){
    return basename(name);
}

unsigned short getFilenameLength(char *name){
    unsigned short size = sizeof(name) / sizeof(char*)
    return size;
}