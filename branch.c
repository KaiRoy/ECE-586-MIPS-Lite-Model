/*

Timing Simulator

Incorporates Memory Image Handling to deal with Memory
-TIMING STUFF CLEARLY MARKED

*/

/*Header Files*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

/*Macros*/
#define MAXLEN 10 //8 hex values on a given line, room for the null character '\0', and room for the newline character
#define LINECOUNTMAX 1024 //Only 1024 lines from the memory image can be read
#define FILENAMESIZE 60 //60 characters allowed for the file name
#define OPCODESIZE 6
#define REGSIZE 5
#define IMMEDIATESIZE 16

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
int TextToHex(char value); //converts a character to an integer value
int jmp = 0;
/*Enumerations*/
enum opcode {
	ADD 	= 0x00, //000000
	ADDI 	= 0x01, //000001
	SUB 	= 0x02, //000010
	SUBI 	= 0x03, //000011
	MUL 	= 0x04, //000100
	MULI 	= 0x05, //000101
	OR 		= 0x06, //000110
	ORI 	= 0x07, //000111
	AND 	= 0x08, //001000
	ANDI 	= 0x09, //001001
	XOR 	= 0x0A, //001010
	XORI 	= 0x0B, //001011
	LDW 	= 0x0C, //001100
	STW 	= 0x0D, //001101
	BZ 		= 0x0E, //001110
	BEQ 	= 0x0F, //001111
	JR 		= 0x10, //010000
	HALT 	= 0x11  //010001
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

/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*                 TIMING STUFF STARTS HERE                   */
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/

/*Macros*/
#define PIPELINESIZE 5
#define RTYPE 99
#define ITYPE 100


/*Structs*/ //must come before timing simulator global variables
typedef struct
{
    enum opcode OpCode;
    unsigned int Rs;
    unsigned int Rt;
    unsigned int Rd;
    signed int Immediate;

    bool RegDst;
    bool WriteReg;
    bool ReadReg1;
    bool ReadReg2;
} instruction;

/*Global Variables*/
int clk_cnt = 0; //only one clock
//int clk_cnt_with_forwarding = 0;

int stalls_no_forwarding = 0;
int stalls_with_forwarding = 0;

int ID_EXE_flag;
int EXE_MEM_flag;

//instruction *pipeline_stage_ptr[PIPELINESIZE]; //array of pointers to structs. records what the stage contains, i.e. what is in IF stage, ID stage, EXE stage, etc.
//instruction pipeline[PIPELINESIZE]; //circular buffer, each entry is an instruction

/*Function Prototypes*/
void init_pipeline(instruction *IF, instruction *ID, instruction *EXE, instruction *MEM, instruction *WB);
int instruction_type(enum opcode OpCode); //returns the type of instruction. R type or I type


/*Main function for this code*/
int main()
{
    /*
    //Get string name from user
    char file_name[FILENAMESIZE];
    printf("Enter the name of the memory image textfile: (Example: MemoryImage.txt)\n");
    scanf("%s", file_name);
    file_name[strcspn(file_name, "\n")] = '\0'; //replaces newline will null character
    */

	FILE *fp;   //file pointer
	fp = fopen("MemoryImage.txt", "r"); //pass the address of the string storing the file name
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
    //type_components components;

    //Extract the string from the text file and place it into the memory array
    while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
    {
        line_count++; //increment the line counter for each line read

        //Stops reading from memory image if more than 1024 lines are in the memory image file
        if (line_count >= LINECOUNTMAX)
        {
            printf("\n");
            printf("WARNING! Exceeded 4kB memory state, lines after 1024 will no longer be read\n\n");
            break;
        }

        line[strcspn(line, "\n")] = '\0'; //replaces newline will null character

        if (!containshex(line)) //checking for black lines
        {
            line_count--; //Blank lines don't count for the line counter
            continue;
        }

        //Fill memory
        mem[mem_index] = Mem_Image_Handler(line);
        mem_index++;
    }

    /**************************************************************/
    /**************************************************************/
    /**************************************************************/
    /**************************************************************/
    /*                 TIMING STUFF STARTS HERE                   */
    /**************************************************************/
    /**************************************************************/
    /**************************************************************/
    /**************************************************************/


    //Declaring structs for each pipeline stage
    instruction new_instruction;
    instruction IF_stage;
    instruction ID_stage;
    instruction EXE_stage;
    instruction MEM_stage;
    instruction WB_stage;

    //Initialize stages of the pipeline
    init_pipeline(&IF_stage, &ID_stage, &EXE_stage, &MEM_stage, &WB_stage);

    //Pointers to each stage in the pipeline
    /*
    pipeline_stage_ptr[0] = &IF_stage;
    pipeline_stage_ptr[1] = &ID_stage;
    pipeline_stage_ptr[2] = &EXE_stage;
    pipeline_stage_ptr[3] = &MEM_stage;
    pipeline_stage_ptr[4] = &WB_stage;
    */



    //Iterate through the memory array, writing to registers/components line by line
    running: for (int i = 0; i < line_count; i++)
    {
    	/**************ADDED**************/
    	
    	jump:
    	/********************************/
        //Get the opcode, place values in components
        new_instruction.OpCode = (mem[i] & Opcode_Mask) >> 26;
        if (instruction_type(new_instruction.OpCode) == RTYPE)
        {
            new_instruction.Rs = (mem[i] & Rs_Mask) >> 21;
            new_instruction.Rt = (mem[i] & Rt_Mask) >> 16;
            new_instruction.Rd = (mem[i] & Rd_Mask) >> 11;
        }
        else if (instruction_type(new_instruction.OpCode) == ITYPE)
        {
            new_instruction.Rs = (mem[i] & Rs_Mask) >> 21;
            new_instruction.Rt = (mem[i] & Rt_Mask) >> 16;
            new_instruction.Immediate = (mem[i] & Immediate_Mask);
        }
        else
        {
            printf("ERROR: Invalid Opcode\n");
        }

        //We now have the new instruction struct filled, next place it in the pipeline

        //Move instructions to the point where hazard detection can begin
        while(clk_cnt < 3)
        {   //fill the pipeline to the point we can start to detect hazards
            //Order is important here
             WB_stage = MEM_stage; //MEM to WB
             MEM_stage = EXE_stage; //EXE to MEM
             EXE_stage = ID_stage; //ID to EXE
             ID_stage = IF_stage; //IF to ID
             IF_stage = new_instruction; //new instruction enters the pipeline

             clk_cnt++;
             goto running;
        }
        //reset  the flags for hazard detection
        ID_EXE_flag = 0;
        EXE_MEM_flag = 0;

        hazard: //checking ID stage compared to EXE and MEM
            if(ID_stage.OpCode == RTYPE )
            {
                if(1) //check exe stage
                {
                    if(EXE_stage.OpCode == RTYPE)
                    {
                        //compare exe destination with id source
                        if (EXE_stage.Rd == ID_stage.Rs || EXE_stage.Rd == ID_stage.Rt)
                        {
                            ID_EXE_flag = 1;
                        }
                    }
                    else //instruction_type(EXE_stage) == ITYPE
                    {
/************************ Added ***************************************/
                        if(EXE_stage.Opcode == BEQ||EXE_stage.Opcode == BZ){
                        	branchHandle(EXE_stage, i);
                        	i = jmp;
                        	goto jump;
                        }
/************************ End *******************************************/
                        //compare exe destination with id source
                        if (EXE_stage.Rt == ID_stage.Rs || EXE_stage.Rt == ID_stage.Rs)
                        {
                            ID_EXE_flag = 1;
                        }

                    }
                }
                if(1) //check mem stage
                {
                    if(MEM_stage.OpCode == RTYPE)
                    {
                        //compare mem destination with id source
                        if (MEM_stage.Rd == ID_stage.Rs || MEM_stage.Rd == ID_stage.Rt)
                        {
                            EXE_MEM_flag = 1;
                        }
                    }
                    else //instruction_type(mem_stage) == ITYPE
                    {
                        //compare mem destination with id source
                        if (MEM_stage.Rt == ID_stage.Rs || MEM_stage.Rt == ID_stage.Rs)
                        {
                            EXE_MEM_flag = 1;
                        }
                    }
                }

            }
            else //instruction_type(ID_stage) == ITYPE
            {
                if(1) //check exe stage
                {
                    if(EXE_stage.OpCode == RTYPE)
                    {
                        //compare exe destination with id source
                        if (EXE_stage.Rd == ID_stage.Rs)
                        {
                            ID_EXE_flag = 1;
                        }
                    }
                    else //instruction_type(EXE_stage) == ITYPE
                    {
                        //compare exe destination with id source
                        if (EXE_stage.Rt == ID_stage.Rs)
                        {
                            ID_EXE_flag = 1;
                        }
                    }
                }
                if(1) //check mem stage
                {
                    if(MEM_stage.OpCode == RTYPE)
                    {
                        //compare mem destination with id source
                        if (MEM_stage.Rd == ID_stage.Rs)
                        {
                            EXE_MEM_flag = 1;
                        }
                    }
                    else //instruction_type(mem_stage) == ITYPE
                    {
                        //compare mem destination with id source
                        if (MEM_stage.Rt == ID_stage.Rs)
                        {
                            EXE_MEM_flag = 1;
                        }
                    }
                }
            }

            //compute stall cycles from hazard
            if(ID_EXE_flag == 1 && EXE_MEM_flag == 0)
            {
                stalls_no_forwarding += 2;
            }
            else if(ID_EXE_flag == 1 && EXE_MEM_flag == 1)
            {
                stalls_no_forwarding += 2;
            }
            else if(ID_EXE_flag == 0 && EXE_MEM_flag == 1)
            {
                stalls_no_forwarding += 1;
            }
            else //no hazard
            {
                goto normal;
            }

        branch: //branch
            //

        normal:
            clk_cnt++;

            //Shift instructions through pipeline. Order is important here
            WB_stage = MEM_stage; //MEM to WB
            MEM_stage = EXE_stage; //EXE to MEM
            EXE_stage = ID_stage; //ID to EXE
            ID_stage = IF_stage; //IF to ID
            IF_stage = new_instruction; //new instruction enters the pipeline
    }

    //Close the file
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
	int exp = 7; //exponent for converting
	int mem_image_hex = 0;

	//Converting the string to an integer
    for (int i = 0; i < 8; i++)
    {
        mem_image_hex = mem_image_hex + (TextToHex(line[i]) * pow(16,exp));
        exp--;
    }

    return mem_image_hex;
}


/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*                 TIMING STUFF STARTS HERE                   */
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/

//Initialize stage structs
void init_pipeline(instruction *IF, instruction *ID, instruction *EXE, instruction *MEM, instruction *WB)
{
    //*(IF).OpCode same as IF->OpCode
    IF->OpCode = 0x00;
    IF->Rs = 33; //impossible value for a register in mips
    IF->Rt = 34;
    IF->Rd = 35;
    IF->Immediate = 0;

    ID->OpCode = 0x00;
    ID->Rs = 36; //impossible value for a register in mips
    ID->Rt = 37;
    ID->Rd = 38;
    ID->Immediate = 0;

    EXE->OpCode = 0x00;
    EXE->Rs = 39; //impossible value for a register in mips
    EXE->Rt = 40;
    EXE->Rd = 41;
    EXE->Immediate = 0;

    MEM->OpCode = 0x00;
    MEM->Rs = 42; //impossible value for a register in mips
    MEM->Rt = 43;
    MEM->Rd = 44;
    MEM->Immediate = 0;

    WB->OpCode = 0x00;
    WB->Rs = 45; //impossible value for a register in mips
    WB->Rt = 46;
    WB->Rd = 47;
    WB->Immediate = 0;
}

int instruction_type(enum opcode OpCode)
{
    if (OpCode == ADD ||
        OpCode == SUB ||
        OpCode == MUL ||
        OpCode == OR ||
        OpCode == AND ||
        OpCode == XOR)
        {
            return RTYPE;
        }
    else if (OpCode == ADDI ||
             OpCode == SUBI ||
             OpCode == MULI ||
             OpCode == ORI ||
             OpCode == ANDI ||
             OpCode == XORI ||
             OpCode == LDW ||
             OpCode == STW ||
             OpCode == BZ ||
             OpCode == BEQ ||
             OpCode == JR ||
             OpCode == HALT)
        {
            return ITYPE;
        }
        else
        {
            printf("ERROR: Invalid Opcode\n");
            exit(1);
        }
}

void branchHandle(instruction EXE, int lineCount){
	//for BZ if the contexts of register Rs are zero, then branch to the xth instruction from
	//the current 
	if (EXE.OpCode==BZ){
		if(EXE.Rs == 0){
			jump = lineCount + 4 + EXE.Immediate;
			clk_cnt=clk_cnt+3;
		}
	}
	
	//if BEQ  then compare the contents of registers Rs and Rt. If they are equal, then
	//branch to the xth instruction from the current instruction
	if (EXE.OpCode==BEQ){
		if (EXE.Rs == EXE.Rt){
			jump = lineCount + 4 + EXE.Immediate;
			clk_cnt=clk_cnt+3;
			
		}
	}
}

