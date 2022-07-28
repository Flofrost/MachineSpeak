#include <stdio.h>
#include "src/mchnSpk.h"

char* outfilename = "/tmp/machinespeak/out.wav";
unsigned char option = 0x20;
unsigned int interval = 16;
const unsigned char wavHeader[44] = {0x52,0x49,0x46,0x46,0x56,0x30,0x00,0x00,0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x40,0x1f,0x00,0x00,0x40,0x1f,0x00,0x00,0x01,0x00,0x08,0x00,0x64,0x61,0x74,0x61,0x31,0x30,0x00,0x00};

int main(int argc, char** argv){

    char command[512];
    char* msgin = argParser(argc, argv);

    if(option & VERBOSE){
        printf("Flags : ");
        if(option & QUIET) printf("Quiet ");
        if(option & VERBOSE) printf("Verbose ");
        if(!(option & NOISE)) printf("No-Noise ");
        printf("\nInterval : %dms",interval*32);
        printf("\nMode : ");
        switch(option & MODE){
            case 0:
                printf("Nato");
                break;
            case 1:
                printf("Hex-ASCII");
                break;
            case 2:
                printf("Morse");
                break;
        }
        printf("\nOutput File : %s\n", outfilename);
        printf("Message : %s\n", msgin);
    }

    char* msgout = messageMallocator(msgin);

    transcriptMessage(0,msgin,msgout);

    if(option & VERBOSE) printf("Transcripted Message : %s\n",msgout);

    sprintf(command,"rm %s",outfilename);
    system(command);

    for(int i = 0 ; msgout[i] ; i++) writeFile(msgout[i]);

    endFile();

    if(!(option & QUIET)){
        sprintf(command,"aplay -q %s 2> /dev/null",outfilename);
        system(command);
    }

    return 0;
}