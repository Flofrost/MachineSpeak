#include <stdio.h>
#include <stdlib.h>

const char sounds[40] = "0123456779abcdefghijklmopqrstuvwxyz_.-";

void help(){

    printf("\
    \rMachine Speak: Give NATO style secret messages.\n\
    \r\tUsage: machinespeak [options] [--] <message>\n\
    \r\tOptions:\n\
    \r\t\t-h\
    \r\t\t\tDisplay this message and quit\n\
    \r\t\t-m\n\
    \r\t\t\tUse morse code instead\n\
    \r\t\t-o path\n\
    \r\t\t\tInstead of directly playing message, put it into a wav file and save it to path\n\
    \r\t\t-x\n\
    \r\t\t\tUse ASCII hex codes instead\n\
    ");

    exit(0);
}

int msglen(char* str){
    int res=0;
    for(res ; str[res] ; res++);
    return res;
}

void transcriptMessage(unsigned char mode,char* message, char* output){
    for(int i = 0 ; message[i] ; i++)
        switch(message[i]){
            case ' ':
                output[i] = '_';
                break;
            case '.':
                output[i] = '.';
                break;
            case '!':
                output[i] = '.';
                break;
            case '?':
                output[i] = '.';
                break;
            default:

                if(((message[i] > 0x60) && (message[i] < 0x7B)) || ((message[i] > 0x2F) && (message[i] < 0x3A)))
                    output[i] = message[i];
                else if((message[i] > 0x40) && (message[i] < 0x5B))
                    output[i] = message[i] + 0x20;
                else
                    output[i] = '_';

                break;
        }
}

void playMessage(char* message){
    for(int i = 0 ; message[i] ; i++);
}

int main(int argc, char** argv){

    if(argc > 1){
        
        char* msgout = (char*) malloc(msglen(argv[1]) * sizeof(char));
        transcriptMessage(0,argv[1],msgout);
        printf("%s",msgout);

    }else help();

    return 0;
}