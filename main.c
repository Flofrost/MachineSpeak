#include <stdio.h>
#include <stdlib.h>

#define SOUND_PATH "./audio"

const char sounds[40] = "0123456779abcdefghijklmopqrstuvwxyz_.-";
const unsigned char wavHeader[40] = {82,73,70,70,40,74,1,0,87,65,86,69,102,109,116,32,16,0,0,0,1,0,1,0,68,172,0,0,136,88,1,0,2,0,16,0,100,97,116,97};

void help(){

    printf("\
    \rMachine Speak: Give NATO style secret messages.\n\
    \r\tUsage: machinespeak [options] [--] <message>\n\
    \r\tOptions:\n\
    \r\t\t-h     \t\tDisplay this message and quit\n\
    \r\t\t-m     \t\tUse morse code instead\n\
    \r\t\t-o path\t\tSave message to path (.wav will be automatically appeneded)\n\
    \r\t\t-q     \t\tDo not play message. (handy with -o)\n\
    \r\t\t-x     \t\tUse ASCII hex codes instead\n\
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

void writeFile(char sound){
    char command[100];
    sprintf(command, "exit $(ls %s | grep -P \"%c\\.\\d\\.wav\" | wc -l)",SOUND_PATH,sound);
    int nfiles = system(command)>>8;
    FILE* test = fopen(SOUND_PATH"/test.wav","w");
    fwrite(wavHeader,1,40,test);
    fclose(test);
}

int main(int argc, char** argv){

    if(argc > 1){
        
        char* msgout = (char*) malloc(msglen(argv[1]) * sizeof(char));
        transcriptMessage(0,argv[1],msgout);
        printf("%s\n",msgout);
        writeFile('1');

    }else help();

    return 0;
}