#include <stdio.h>
#include <stdlib.h>

#define ok printf("ok\n");
#define SOUND_PATH "./audio"
#define system(x) (system(x) >> 8)

#define QUIET   0x80
#define VERBOSE 0x40
#define MODE    0x03

char* outfilename = "/tmp/machinespeak/out.wav";
unsigned char option = 0x00; // 0bQVXXXXMM   Q = quiet  V = Verbose   M = mode (Modes: 0 nato, 1 hex, 2 morse)
const char sounds[40] = "0123456779abcdefghijklmopqrstuvwxyz_.-";
const unsigned char wavHeader[44] = {0x52,0x49,0x46,0x46,0x56,0x30,0x00,0x00,0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x40,0x1f,0x00,0x00,0x40,0x1f,0x00,0x00,0x01,0x00,0x08,0x00,0x64,0x61,0x74,0x61,0x31,0x30,0x00,0x00};

void help(){

    printf("\
    \rMachine Speak: Give NATO style secret messages.\n\
    \r\tUsage: machinespeak [options] <message>\n\
    \r\tOptions:\n\
    \r\t\t-h     \t\tDisplay this message and quit\n\
    \r\t\t-m     \t\tUse morse code instead\n\
    \r\t\t-o path\t\tSave message to path\n\
    \r\t\t-q     \t\tDo not play message. (handy with -o)\n\
    \r\t\t-v     \t\tVerbose, for debugging.\n\
    \r\t\t-x     \t\tUse ASCII hex codes instead\n\
    ");

    exit(0);
}

int msglen(char* str){
    int res=0;
    for(res ; str[res] ; res++);
    return res;
}

char* argParser(int argc, char** argv){
    if(argc > 1){
        for(int i = 1 ; i < argc ; i++){
            if((argv[i][0] == '-') && (argv[i][1] == 'o'))
                outfilename = argv[++i];
            else if(argv[i][0] == '-')
                for(int j = 1 ; argv[i][j] ; j++)
                    switch(argv[i][j]){
                        case 'h':
                            help();
                            break;
                        case 'm':
                            option = (option & 0xFC) | 2;
                            break;
                        case 'q':
                            option |= 0x80;
                            break;
                        case 'v':
                            option |= 0x40;
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
            return (char*) malloc(msglen(msg) * sizeof(char));
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

    char command[512];
    char* msgin = argParser(argc, argv);

    if(option & VERBOSE){
        printf("Flags : ");
        if(option & QUIET) printf("Quiet ");
        if(option & VERBOSE) printf("Verbose ");
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

    sprintf(command,"aplay -q %s 2> /dev/null",outfilename);
    system(command);

    return 0;
}