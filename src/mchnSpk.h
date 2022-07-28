#ifndef MCHNSPK_H
#define MCHNSPK_H

#include <stdio.h>
#include <stdlib.h>

#define ok printf("ok\n");
#define SOUND_PATH "./audio"
#define system(x) (system(x) >> 8)

#define QUIET   0x80
#define VERBOSE 0x40
#define NOISE   0x20
#define MODE    0x03

#define AMP     30


extern char* outfilename;
// 0bQVNXXXMM   Q = quiet  V = Verbose  N = Noise   M = mode (Modes: 0 nato, 1 hex, 2 morse)
extern unsigned char option;
extern unsigned int interval;
extern const unsigned char wavHeader[44];


void help();
char* argParser(int argc, char** argv);
char* messageMallocator(char* msg);
void transcriptMessage(unsigned char mode,char* message, char* output);
void writeFile(char sound);
void endFile();


#endif