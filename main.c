#include <stdio.h>
#include <stdlib.h>

#define ok printf("ok\n");
#define SOUND_PATH "./audio"
#define system(x) (system(x) >> 8)

// char outfilename[256] = "/tmp/machinespeak/out.wav";
char outfilename[256] = "audio/out.wav";
const char sounds[40] = "0123456779abcdefghijklmopqrstuvwxyz_.-";
const unsigned char wavHeader[44] = {0x52,0x49,0x46,0x46,0x00,0x00,0x00,0x00,0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x44,0xac,0x00,0x00,0x88,0x58,0x01,0x00,0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,0x00,0x00,0x00,0x00};

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

    char command[512];
    char infilename[256];
    sprintf(command, "exit $(ls %s | grep -P \"%c\\.\\d\\.wav\" | wc -l)",SOUND_PATH,sound);
    int nfile = system(command);
    nfile = rand() % nfile;
    sprintf(infilename,"%s/%c.%d.wav",SOUND_PATH,sound,nfile);

    sprintf(command,"[ -s \"%s\" ]",outfilename);
    if(system(command)){
        sprintf(command,"mkdir -p $(dirname %s)",outfilename);
        system(command);
        FILE* outfile = fopen(outfilename,"w");
        fwrite(wavHeader,1,40,outfile);
        fclose(outfile);
    }

    FILE* outfile = fopen(outfilename,"a");
    FILE* infile = fopen(infilename,"r");
    fseek(infile,40,SEEK_SET);
    unsigned char buff[256];
    int nbytes = 256;

    while(!feof(infile)){
        nbytes = fread(buff,1,nbytes,infile);
        fwrite(buff,1,nbytes,outfile);
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

int main(int argc, char** argv){

    if(argc > 1){
        
        char* msgout = (char*) malloc(msglen(argv[1]) * sizeof(char));
        char command[512];

        transcriptMessage(0,argv[1],msgout);

        sprintf(command,"rm %s",outfilename);
        system(command);

        for(int i = 0 ; argv[1][i] ; i++) writeFile(argv[1][i]);

        endFile();

    }else help();

    return 0;
}