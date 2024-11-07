#include <stdint.h>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <getopt.h> 
#include <fcntl.h>
#include <limits.h>
#include <ctype.h>

#ifndef MAX_BUFFER_LEN 
# define MAX_BUFFER_LEN 16
#endif // MAX_BUFFER_LEN

char *binStr(char byte, char *binOut); //bin char to 8 bit string
void isVerbose(int verbose, char *inputFile, char *outputFile, char *format, int noAscii); //do verbose if flag
void dump(FILE *input, FILE *output, int noAscii, char *format, char *spaces); //parse and print the file

int main(int argc, char *argv[]){
    int noAscii = 0; //flag for -A option
    int verbose = 0; //flag for verbose
    int opt = -1; // The return value from each call to getopt().
    FILE *input= stdin; //pointer for input
    FILE *output = stdout; //pointer for output, set to stdout by default
    FILE *additional = NULL; //pointer for additional input
    char format[100] = "%02hhx "; //string to hold the format specifier based on options, set to hex by default
    char spaces[20] = "   "; //string to hold spaces

    char outputFile[100] = ""; //name of output file if specified
    char inputFile[100] = ""; //name of input file 

    //loop and switch case for getopt()
    while ((opt = getopt(argc, argv, "I:O:xdobAvh")) != -1){
        switch (opt){
            case 'I': //filename as argument for input, if not specified, stdin is used
                strcpy(inputFile, optarg);
            break;

            case 'O': //filename as argument for output, if not specified, stdout is used
                strcpy(outputFile, optarg);
            break;

            case 'x': //hex output
                strcpy(format, "%02hhx ");
                strcpy(spaces, "   ");
            break;

            case 'd': //decimal output
                strcpy(format, "%3hhu ");
                strcpy(spaces, "    ");
            break;

            case 'o': //octal output
                strcpy(format, "%03hho ");
                strcpy(spaces, "    ");
            break;

            case 'b': //binary output
                strcpy(format, "%8s ");
                strcpy(spaces, "         ");
            break;

            case 'A': //skip ASCII output
                noAscii = 1;
            break;

            case 'v': //verbose mode
                verbose = 1;
            break;

            case 'h': //help message
            printf("\t\tvhex [I:O:xdobAhv]\n");
            printf("-I <ifile>: Name of (first) input file.\n");
            printf("\tIf not specified, stdin is used.\n");
            printf("-O <ofile>: Name of output file.\n");
            printf("\tIf not specified, stdout is used.\n");
            printf("-x        : Output is in hex (default).\n");
            printf("-d        : Output is in decimal.\n");
            printf("-o        : Output is in octal.\n");
            printf("-b        : Output is in binary.\n");
            printf("-A        : Don't show ASCII output.\n");
            printf("-v        : Enable verbose level.\n");
            printf("-h        : Show this amazing help.\n");
            return EXIT_SUCCESS;

            default:
            fprintf(stderr, "\t\tvhex [I:O:xdobAhv]\n");
            fprintf(stderr, "-I <ifile>: Name of (first) input file.\n");
            fprintf(stderr, "\tIf not specified, stdin is used.\n");
            fprintf(stderr, "-O <ofile>: Name of output file.\n");
            fprintf(stderr, "\tIf not specified, stdout is used.\n");
            fprintf(stderr, "-x        : Output is in hex (default).\n");
            fprintf(stderr, "-d        : Output is in decimal.\n");
            fprintf(stderr, "-o        : Output is in octal.\n");
            fprintf(stderr, "-b        : Output is in binary.\n");
            fprintf(stderr, "-A        : Don't show ASCII output.\n");
            fprintf(stderr, "-v        : Enable verbose level.\n");
            fprintf(stderr, "-h        : Show this amazing help.\n");
            return EXIT_SUCCESS;
            break;
        }
    }
    //attempt to open input file (-I flag argument)
    if(strcmp(inputFile, "") != 0)
        //inFile = open(inputFile, O_RDONLY);
        input = fopen(inputFile, "r");
    //attempt to open output file (-O flag argument)
    if(strcmp(outputFile, "") != 0)
        output = fopen(outputFile, "w");

    isVerbose(verbose, inputFile, outputFile, format, noAscii); //call the isVerbose function to print verbose options 
    dump(input, output, noAscii, format, spaces); //surf through the first file opened at input

    if (optind < argc)
    {
        int j;

        fprintf(stderr, "\nThis is what remains on the command line:\n");
        for(j = optind; j < argc; j++)
        {
            fprintf(output,"*****\nPROCESSING EXTRA INPUT FILE: %s\n", argv[j]);
            additional = fopen(argv[j], "r"); //attempt to open additional files on the command line
            dump(additional, output, noAscii, format, spaces); //run again at each remaining file on the command line
            if(additional) fclose(additional); //close the additional file
        }
    }

    if(input) fclose(input); //close original input file
    if(output) fclose(output); //close output file
    return EXIT_SUCCESS;
}

//string conversion function
char *binStr(char byte, char *binOut){
    uint8_t mask = 0x1 << (CHAR_BIT - 1);
    int i = 0;
    while (mask){
        binOut[i++] = (mask & byte) ? '1' : '0';
        mask >>= 1;
    }
    return binOut;
}


//main parse and print function
void dump(FILE *input, FILE *output, int noAscii, char *format, char *spaces){
    char binString[9] = "00000000";
    char buffer[MAX_BUFFER_LEN] = {'\0'}; //buffer for read()
    char ascii[MAX_BUFFER_LEN + 1] = {'\0'};//string of ascii printable chars (or '.')
    long double bytesRead = 0; //counter for bytes read
    ssize_t totalBytes = 0; //total bytes (offset from beginning)
    while((bytesRead = fread(buffer, 1, MAX_BUFFER_LEN, input)) > 0 ) {
        fprintf(output, "%08lx  ", totalBytes); //print current byte count
        for(int i = 0; i < MAX_BUFFER_LEN; ++i){
            if(i == 8)
                fprintf(output, " ");
            if(i >= bytesRead){
                fprintf(output, "%s", spaces);
            }
            else if(strcmp(format, "%8s ") == 0){
                binStr(buffer[i], binString);
                fprintf(output, format, binString);
                strcpy(binString, "00000000");
            }
            else fprintf(output, format, buffer[i]);
        }

        if(!noAscii){ //if the -A flag has not been recieved
            for(int i = 0; i < MAX_BUFFER_LEN; ++i){
                if(isprint(buffer[i]))
                    ascii[i] = buffer[i];
                
                else if(i >= bytesRead){ //terminate the ascii string if bytesRead has been reached 
                    ascii[i] = '\0';
                    break;
                }
                else //if not printable, print a '.'
                    ascii[i] = '.';
            }

            fprintf(output, " |%s|\n", ascii); //format and print the ascii string 
        }

        else
            fprintf(output, "\n"); //if the -A flag was recieved, start a new line 

        totalBytes += bytesRead; //update byte count
    }
}

//takes specs and verbose flag and prints verbose info
void isVerbose(int verbose, char *inputFile, char *outputFile, char *format, int noAscii){
    if(verbose){
        fprintf(stderr, "Configuration:\n");
        if(strcmp(inputFile, "") != 0)
            fprintf(stderr, "\tInput from:    %s\n", inputFile);
        else
            fprintf(stderr, "\tInput from:   stdin\n");

        if(strcmp(outputFile, "") != 0)
            fprintf(stderr, "\tOutput to:     %s\n", outputFile);
        else
            fprintf(stderr, "\tOutput to:     stdout\n");

        if(noAscii)
            fprintf(stderr, "\tSkip ASCII:    true\n");
        else
            fprintf(stderr, "\tSkip ASCII:    false\n");

        fprintf(stderr, "\tOutput mode:   ");

        if(strcmp("%02hhx ", format) == 0)
            fprintf(stderr, "HEX\n");

        else if(strcmp("%3hhu ", format) == 0)
            fprintf(stderr, "DECIMAL\n");

        else if(strcmp("%03hho ", format) == 0)
            fprintf(stderr, "OCTAL\n");

        else
            fprintf(stderr, "BINARY\n");
    }
}
