#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mchnSpk.h"

void help(){

    printf("\
    \rMachine Speak: Generate Machine style secret messages.\n\
    \r\tUsage: machinespeak [options] <message>\n\
    \r\tOptions:\n\
    \r\t\t-h          \t\tDisplay this message and quit\n\
    \r\t\t-i interval \t\tInterval between samples in milliseconds (default 512ms) (multiple of 32ms)\n\
    \r\t\t-m          \t\tUse morse code instead\n\
    \r\t\t-n          \t\tNo noise\n\
    \r\t\t-o path     \t\tSave message to path\n\
    \r\t\t-q          \t\tDo not play message. (handy with -o)\n\
    \r\t\t-v          \t\tVerbose, for debugging.\n\
    \r\t\t-x          \t\tUse ASCII hex codes instead\n\
    ");

    exit(0);
}

char* argParser(int argc, char** argv){
    if(argc > 1){
        for(int i = 1 ; i < argc ; i++){
            if((argv[i][0] == '-') && (argv[i][1] == 'o'))
                outfilename = argv[++i];
            else if((argv[i][0] == '-') && (argv[i][1] == 'i')){
                sscanf(argv[++i],"%d",&interval);
                interval /= 32;
            }
            else if(argv[i][0] == '-')
                for(int j = 1 ; argv[i][j] ; j++)
                    switch(argv[i][j]){
                        case 'h':
                            help();
                            break;
                        case 'm':
                            option = (option & 0xFC) | 2;
                            break;
                        case 'n':
                            option &= ~NOISE;
                            break;
                        case 'q':
                            option |= QUIET;
                            break;
                        case 'v':
                            option |= VERBOSE;
                            break;
                        case 'x':
                            option = (option & 0xFC) | 1;
                            break;
                    }
            else
                return argv[i];
        }
    }else help();
}

char* messageMallocator(char* msg){
    switch(option & MODE){
        case 0:
            return (char*) malloc(strlen(msg) * sizeof(char));
            break;
        case 1:
            break;
        case 2:
            break;
    }
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
                output[i] = '-';
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

void writeFile(char sound){

    char command[512];
    char infilename[256];
    if(sound != '.' && sound != '-') sprintf(command, "exit $(ls -A %s | grep -P \"%c\\.\\d\\.wav\" | wc -l)",SOUND_PATH,sound);
    else sprintf(command, "exit $(ls -A %s | grep -P \"\\%c\\.\\d\\.wav\" | wc -l)",SOUND_PATH,sound);
    int nfile = system(command);
    nfile = rand() % nfile;
    sprintf(infilename,"%s/%c.%d.wav",SOUND_PATH,sound,nfile);

    sprintf(command,"[ -s \"%s\" ]",outfilename);
    if(system(command)){
        sprintf(command,"mkdir -p $(dirname %s)",outfilename);
        system(command);
        FILE* outfile = fopen(outfilename,"w");
        fwrite(wavHeader,1,44,outfile);
        fclose(outfile);
    }

    FILE* outfile = fopen(outfilename,"a");
    FILE* infile = fopen(infilename,"r");
    fseek(infile,44,SEEK_SET);
    unsigned char buff[256];
    int nbytes = 256;

    while(!feof(infile)){
        nbytes = fread(buff,1,nbytes,infile);
        if(option & NOISE) for(int ii = 0 ; ii < nbytes ; ii++){
            int tmp = buff[ii] + ((rand() % AMP) - (AMP >> 1));
            if(tmp > 255) buff[ii] = 255;
            else if(tmp < 0) buff[ii] = 0;
            else buff[ii] = tmp;
        }
        fwrite(buff,1,nbytes,outfile);
    }

    for(int i = 0; i < interval; i++){
        if(option & NOISE) for(int ii = 0 ; ii < 256 ; ii++){
            int tmp = 127 + (rand() % AMP) - (AMP >> 1);
            if(tmp > 255) buff[ii] = 255;
            else if(tmp < 0) buff[ii] = 0;
            else buff[ii] = tmp;
        }else for(int ii = 0 ; ii < 256 ; ii++) buff[ii] = 127;
        fwrite(buff,1,256,outfile);
    }

    fclose(infile);
    fclose(outfile);

}

void endFile(){
    int32_t size = 0;
    FILE* outfile = fopen(outfilename,"r+");
    while(fgetc(outfile) != EOF) size++;

    size -= 8;
    fseek(outfile,4,SEEK_SET);
    fwrite(&size,4,1,outfile);
    size -= 36;
    fseek(outfile,40,SEEK_SET);
    fwrite(&size,4,1,outfile);

    fclose(outfile);
}