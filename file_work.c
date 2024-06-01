/*

Handling the Memory Image

Assumes the memory image text file will contain good data:
-Only hex values
-Each line will be comprised of exactly 8 hex values, nothing more or less.

*/

/*Header Files*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*Macros*/
#define MAXLEN 10 //8 hex values on a given line, room for the null character '\0', and room for the newline character
#define LINELEN 60 //to deal with any unexpected values in a line of the memory image file
#define LINECOUNTMAX 1024 //Only 1024 lines from the memory image can be read

/*Global Variables*/
int mem[LINECOUNTMAX]; //array of 1024 integer values
const int Opcode_Mask = 0xFC000000;
const int Rs_Mask = 0x03E00000;
const int Rt_Mask = 0x001F0000;
const int Rd_Mask = 0x0000F800;
const int Immediate_Mask = 0x0000FFFF;

/*Function Prototypes*/
int containshex(char string[]); //checks for blank lines.
int Mem_Image_Handler(char line[]); //Converts a line in the image file to an integer and stores in the memory array
int ishexval(char val); //verifies is a character is a hex value
int TextToHex(char value); //converts a character to an integer value

/*Enumerations*/
enum opcodes {
	ADD 	= 000000,
	ADDI 	= 000001,
	SUB 	= 000010,
	SUBI 	= 000011,
	MUL 	= 000100,
	MULI 	= 000101,
	OR 		= 000110,
	ORI 	= 000111,
	AND 	= 001000,
	ANDI 	= 001001,
	XOR 	= 001010,
	XORI 	= 001011,
	LDW 	= 001100,
	STW 	= 001101,
	BZ 		= 001110,
	BEQ 	= 001111,
	JR 		= 010000,
	HALT 	= 010001
};

/*Structs*/
typedef struct
{
    int Opcode;
    int Rs;
    int Rt;
    int Rd;
    int Immediate;

}type_components;



/*Main function for this code*/
int main()
{
	FILE *fp;   //file pointer
	fp = fopen("MemoryImage.txt", "r");
	if (fp == NULL)
    {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
    //string to store a given trace file line
    char line[MAXLEN];
    int line_count = 0; //Keeps track of how many lines are read from the memory image file (max is 1024)
    int mem_index = 0; //index for memory, used instead of line_count because we want to count lines starting at 1 and not 0

    //Struct declaration
    type_components components;

    while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
    {
        printf("the line we got was: %s",line);
        line_count++; //increment the line counter for each line read

        //Stops reading from memory image if more than 1024 lines are in the memory image file
        if (line_count >= LINECOUNTMAX)
        {
            printf("WARNING! Exceeded 4kB memory state, no longer reading from memory image file");
            break;
        }

        line[strcspn(line, "\n")] = 0; //remove newline character from the given line

        if (!containshex(line)) //checking for black lines
        {
            line_count--; //Blank lines don't count for the line counter
            continue;
        }

        printf("line gotten is: %s\n", line);
        for (int i = 0; i < MAXLEN; i++)
        {
            printf("%c", line[i]);
        }
        printf("\n");

        //Fill memory
        mem[mem_index] = Mem_Image_Handler(line);
        mem_index++;
    }

    for (int i = 0; i < line_count; i++)
    {
        components.Opcode = (mem[i] & Opcode_Mask) >> 26;
        if (components.Opcode == ADD || SUB || MUL || OR || AND || XOR)
        {
            components.Rs = (mem[i] & Rs_Mask) >> 21;
            components.Rt = (mem[i] & Rt_Mask) >> 16;
            components.Rd = (mem[i] & Rd_Mask) >> 11;

            //Remove later, printing to check
            printf("Opcode is %x\n", components.Opcode);
            printf("Rs is R%x\n", components.Rs);
            printf("Rt is R%x\n", components.Rt);
            printf("Rd is R%x\n", components.Rd);

        }
        else if (components.Opcode == ADDI || SUBI || MULI || ORI || ANDI || XORI || LDW || STW || BZ || BEQ || JR || HALT)
        {
            components.Rs = (mem[i] & Rs_Mask) >> 21;
            components.Rt = (mem[i] & Rt_Mask) >> 16;
            components.Immediate = (mem[i] & Immediate_Mask);

            //Remove later, printing to check
            printf("Opcode is %x\n", components.Opcode);
            printf("Rs is R%x\n", components.Rs);
            printf("Rt is R%x\n", components.Rt);
            printf("Immediate is %x\n", components.Immediate);
        }
        else
        {
            printf("ERROR: Invalid Opcode\n");
        }
        printf("\n"); //FIXME, remove in final build
    }

    if (fclose(fp) == EOF)
    {
        printf("ERROR: File close not successful\n");
    }

    return 0;
}


/*Functions*/

int containshex(char string[])  //Checks for blank lines in memory image
{
    int does = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if (does == 1)
        {
            break;
        }
        switch(string[i])
        {
            case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'A':
			case 'b':
			case 'B':
			case 'c':
			case 'C':
			case 'd':
			case 'D':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
                does = 1;
                break;
			default:	//no hex value in string
				does = 0;
				break;
        }
    }
    return does;
}

int ishexval(char val)  //verifies is a character is a hex value
{
    switch(val)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'A':
        case 'b':
        case 'B':
        case 'c':
        case 'C':
        case 'd':
        case 'D':
        case 'e':
        case 'E':
        case 'f':
        case 'F':
            return 1;
            break;
        default:	//no hex value in string
            return 0;
            break;
        }
}

/*Converts a character to an integer*/
int TextToHex(char value)
{
	switch (value)
		{
			case '0':
				return 0x0;	//decimal 0	//4'b0000
				break;
			case '1':
				return 0x1;	//decimal 1	//4'b0001
				break;
			case '2':
				return 0x2;	//decimal 2	//4'b0010
				break;
			case '3':
				return 0x3;	//decimal 3	//4'b0011
				break;
			case '4':
				return 0x4;	//decimal 4	//4'b0100
				break;
			case '5':
				return 0x5;	//decimal 5	//4'b0101
				break;
			case '6':
				return 0x6;	//decimal 6	//4'b0110
				break;
			case '7':
				return 0x7;	//decimal 7	//4'b0111
				break;
			case '8':
				return 0x8;	//decimal 8	//4'b1000
				break;
			case '9':
				return 0x9;	//decimal 9	//4'b1001
				break;
			case 'a':
			case 'A':
				return 0xA;	//decimal 10	//4'b1010
				break;
			case 'b':
			case 'B':
				return 0xB;	//decimal 11	//4'b1011
				break;
			case 'c':
			case 'C':
				return 0xC;	//decimal 12	//4'b1100
				break;
			case 'd':
			case 'D':
				return 0xD;	//decimal 13	//4'b1101
				break;
			case 'e':
			case 'E':
				return 0xE;	//decimal 14	//4'b1110
				break;
			case 'f':
			case 'F':
				return 0xF;	//decimal 15	//4'b1111
				break;
			default:	//if this occurs, bad data was put into the trace file
				printf("ERROR: Invalid data from trace file\n");
				exit(1);
				break;
		}
}

/*Function converts a single line of text from the memory image to an integer and stores that integer in a memory array*/
int Mem_Image_Handler(char line[])	//TraceLine is a the address of the first element of a string of unknown length
{
	//Place all character hex values into a string. Ignore all whitespace and non hex values
	char Mem_Image_Text[MAXLEN] = {'x','x','x','x','x','x','x','x','x'};	//initialize to an impossible value
	printf("Mem_Image_Text: %s\n", Mem_Image_Text);
	int hexcount = 0;	//counts the number of hex values in the trace line being examined line (should only be 8)
	int i = 0;
	int j = 0;

	int exp = 7; //exponent for converting

	int mem_image_hex = 0;


	while (line[i] != '\0')	//'\0' is added by fgets
	{
	    if (!ishexval(line[i]))
		{
			i = i + 1; //ignore whitespace, focus only on numerical data
			continue;
		}
		Mem_Image_Text[j] = line[i];
		i = i + 1;
		j = j + 1;
		hexcount = hexcount + 1;
		if (hexcount == 8)	//if we get more than 32 bits of data in a given line of the trace file
		{
		    line[i+1] = '\0'; //not necessary, as fgets adds the newline character
			break;					//ignore data over 32 bits (more than 8 hex values)
		}
	}

	printf("Mem_Image_Text after placement: %s\n", Mem_Image_Text);
	//Converting the string to an integer
    for (int i = 0; i < 8; i++)
    {
        mem_image_hex = mem_image_hex + (TextToHex(Mem_Image_Text[i]) * pow(16,exp));
        exp--;
    }

    return mem_image_hex;
}


