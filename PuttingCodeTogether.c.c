/*
 ============================================================================
 Name        : ece585_mips_lite.c
 Authors     : Kai Roy, Nick Allmeyer, Daisy Perez, Jesus Zavala, Kamal Smith
 Version     :
 Copyright   : Your copyright notice
 Description :
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <stdint.h>
//#include <stdbool.h>


/*************************************************************************************************************
** Macros
*************************************************************************************************************/
#define LINE 1024
//#define SIZE 4*LINE
#define MAX_REGS 32

//From file_work.c------------------------------------------
//8 hex values on a given line, room for the null character '\0', and room for the newline character
#define MAXLEN 10           
#define LINECOUNTMAX 1024 //Only 1024 lines from the memory image can be read

/*************************************************************************************************************
** Model Structure
*************************************************************************************************************/
enum optype {ARITHMETIC, LOGIC, MEMORY, CONTROL};
enum opcode {
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

struct instruction {
	enum optype 	type;
	enum opcode 	code;
	unsigned int 	rs;
	unsigned int 	rt;
	unsigned int 	rd;
	signed int 		imm;
//	bool 	RegDst;
//	bool 	WriteReg;
//	bool 	ReadReg1;
//	bool 	ReadReg2;
};

struct Memory{//Holds array of of lines of memory and the size
	signed int mem[LINE];//This will have the current memory that we are working with
    int state[LINE];
};

struct Registers{//the CPU registers as int array with 32 max registers
    signed int regs[MAX_REGS];
    int state[MAX_REGS];
};

//From file_work.c---------------------------
typedef struct
{
    int opcode;
    int Rs;
    int Rt;
    int Rd;
    int Immediate;

}type_components;

/*************************************************************************************************************
** Globals
*************************************************************************************************************/
int instr_count = 0;
int arith_count = 0;
int logic_count = 0;
int mem_count = 0;
int cntrl_count = 0;

int mode;
int halt = 0;
int pc = 0;

//From file_work.c
int mem[LINECOUNTMAX]; //array of 1024 integer values
const int opcode_Mask = 0xFC000000;
const int Rs_Mask = 0x03E00000;
const int Rt_Mask = 0x001F0000;
const int Rd_Mask = 0x0000F800;
const int Immediate_Mask = 0x0000FFFF;

/*************************************************************************************************************
** Function Prototypes
*************************************************************************************************************/
void menu();
void init(struct Memory *memory, struct Registers *registers);
int func_sim(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_arith(struct instruction instr, struct Registers *registers);
int func_logic(struct instruction instr, struct Registers *registers);
int func_mem(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_cntrl(struct instruction instr, struct Registers *registers);

void print_regs(struct Registers *registers);
void print_mem(struct Memory *memory);

//From file_work.c
int containshex(char string[]); //checks for blank lines.
int Mem_Image_Handler(char line[]); //Converts a line in the image file to an integer and stores in the memory array
int TextToHex(char value); //converts a character to an integer value

/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	//Initial Print
	printf("\nProgram Start\n\n");

	//Initialization
	struct Memory memory;
	struct Registers registers;
	pc = 0;

	init(&memory, &registers);
	
	//string to store a given trace file line from file_work.c
    char line[MAXLEN];
    int line_count = 0; //Keeps track of how many lines are read from the memory image file (max is 1024)
    int mem_index = 0; //index for memory, used instead of line_count because we want to count lines starting at 1 and not 0

    //Struct declaration
    type_components components;
    
/****************************************************************************
** This part below is from file_work.c
****************************************************************************/
	FILE *fp;   //file pointer
	fp = fopen("test.txt", "r");
	if (fp == NULL)
    {
        printf("ERROR: Could not open file\n");
        exit(1);
    }
	
	    //Extract the string from the text file and place it into the memory array
    while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
    {
        line_count++; //increment the line counter for each line read

        //Stops reading from memory image if more than 1024 lines are in the memory image file
        if (line_count >= LINECOUNTMAX)
        {
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

    //Iterate through the memory array, writing to registers/components line by line
    for (int i = 0; i < line_count; i++)
    {
        components.opcode = (mem[i] & opcode_Mask) >> 26;
        if (components.opcode == ADD ||
            components.opcode == SUB ||
            components.opcode == MUL ||
            components.opcode == OR ||
            components.opcode == AND ||
            components.opcode == XOR)
        {
            components.Rs = (mem[i] & Rs_Mask) >> 21;
            components.Rt = (mem[i] & Rt_Mask) >> 16;
            components.Rd = (mem[i] & Rd_Mask) >> 11;
/*
            //Remove later, printing to check
            printf("line: %08x\n", mem[i]);
            printf("line number: %d\n", i+1);
            printf("opcode is %x\n", components.opcode);
            printf("Rs is R%d\n", components.Rs);
            printf("Rt is R%d\n", components.Rt);
            printf("Rd is R%d\n", components.Rd);
*/
        }
        else if (components.opcode == ADDI ||
                 components.opcode == SUBI ||
                 components.opcode == MULI ||
                 components.opcode == ORI ||
                 components.opcode == ANDI ||
                 components.opcode == XORI ||
                 components.opcode == LDW ||
                 components.opcode == STW ||
                 components.opcode == BZ ||
                 components.opcode == BEQ ||
                 components.opcode == JR ||
                 components.opcode == HALT)
        {
            components.Rs = (mem[i] & Rs_Mask) >> 21;
            components.Rt = (mem[i] & Rt_Mask) >> 16;
            components.Immediate = (mem[i] & Immediate_Mask);

/*
            //Remove later, printing to check
            printf("line: %08x\n", mem[i]);
            printf("line number: %d\n", i+1);
            printf("opcode is %x\n", components.opcode);
            printf("Rs is R%d\n", components.Rs);
            printf("Rt is R%d\n", components.Rt);
            printf("Immediate is %04x\n", components.Immediate);
*/
        }
        else
        {
            printf("ERROR: Invalid opcode\n");
        }
/*
        //FIXME, remove in final build, used to space instructions apart in display
        printf("\n");
*/ 
    }

    if (fclose(fp) == EOF)
    {
        printf("ERROR: File close not successful\n");
    }

    //return 0;
	

    //THIS PART BELOW IS FUNC_SIM.c Continued--------------------------
    
	// Testing Instructions
	struct instruction instr1;
	instr1.type = LOGIC;
	instr1.code = OR;
	instr1.rs = 7;
	instr1.rt = 8;
	instr1.rd = 9;
	struct instruction instr2;
	instr2.type = LOGIC;
	instr2.code = AND;
	instr2.rs = 7;
	instr2.rt = 8;
	instr2.rd = 10;
	struct instruction instr3;
	instr3.type = LOGIC;
	instr3.code = XOR;
	instr3.rs = 7;
	instr3.rt = 8;
	instr3.rd = 11;
	struct instruction instr4;
	instr4.type = MEMORY;
	instr4.code = STW;
	instr4.rs = 7;
	instr4.rt = 8;
	instr4.imm = 0;
	struct instruction instr5;
	instr5.type = MEMORY;
	instr5.code = LDW;
	instr5.rs = 7;
	instr5.rt = 20;
	instr5.imm = 50;
	struct instruction instr6;
	instr6.type = CONTROL;
	instr6.code = BZ;
	instr6.rs = 0;
	instr6.rt = 9;
	instr6.imm = 0;
	struct instruction instr7;
	instr7.type = CONTROL;
	instr7.code = JR;
	instr7.rs = 7;
	instr7.rt = 0;
	instr7.imm = 9;


	//Functional Simulator
	printf("Instruction 1:\n");
	func_sim(instr1, &memory, &registers);
	printf("Instruction 2:\n");
	func_sim(instr2, &memory, &registers);
	printf("Instruction 3:\n");
	func_sim(instr3, &memory, &registers);
	printf("Instruction 4:\n");
	func_sim(instr4, &memory, &registers);
	printf("Instruction 5:\n");
	func_sim(instr5, &memory, &registers);
	printf("Instruction 6:\n");
	func_sim(instr6, &memory, &registers);
	printf("Instruction 7:\n");
	func_sim(instr7, &memory, &registers);

	// Print Results
	print_regs(&registers);
	print_mem(&memory);
	printf("Halt = %d\tpc = %d\n", halt, pc);

	printf("\nProgram End\n");

	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: menu
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
void menu() {
	int choice;
	do {
		//Display the options for the mainMenu
		printf("\nPlease select an operating mode");
		printf("\n0: Mode 0");
		printf("\n1: Mode 1");
		printf("\n2: Mode 2");
		printf("\n3: Exit program");

		//User Input here
		printf("\nEnter your choice: ");
		scanf("%d", &choice);

		//Choices for the menu
		if(choice == 0){
			mode = 0;
			break;
		}
		else if(choice == 1){
			mode = 1;
			break;
		}
		else if(choice == 1){
			mode = 2;
			break;
		}
		else if(choice == 3){
			printf("\nThe program will now exit");
			exit(1);
		}
		else {

			printf("\nYou have not entered a valid input\n");
			printf("Please Try again\n");
			continue;
		}
	} while (choice != 3);

	return;
}


void init(struct Memory *memory, struct Registers *registers){
	for(int i = 0; i < LINE; i++) {
		memory->mem[i] = 0;
		memory->state[i] = 0;
	}

	for(int j = 0; j < MAX_REGS; j++) {
		registers->regs[j] = 0;
		registers->state[j] = 0;
	}

	registers->regs[7] = 9;
	registers->regs[8] = 60;

	return;
}




/*************************************************************************************************************
** Functional Simulator Functions
*************************************************************************************************************/
/****************************************************************************
** Function: func_sim
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_sim(struct instruction instr, struct Memory *memory, struct Registers *registers){
	// Switch Case for Optype;
	switch (instr.type) {
		case ARITHMETIC:
			func_arith(instr, registers);
			break;
		case LOGIC:
			func_logic(instr, registers);
			break;
		case MEMORY:
			func_mem(instr, memory, registers);
			break;
		case CONTROL:
			func_cntrl(instr, registers);
			break;
		default:
			printf("Optype does not match - %d\n", instr.type);
			break;
	}

	return 0;
}


/****************************************************************************
** Function: func_sim
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_arith(struct instruction instr, struct Registers *registers){
	switch (instr.code) {
		case ADD:
			registers->regs[instr.rd] = registers->regs[instr.rs] + registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case ADDI:
			registers->regs[instr.rt] = registers->regs[instr.rs] + instr.imm;
			registers->state[instr.rt] = 1;
			break;
		case SUB:
			registers->regs[instr.rd] = registers->regs[instr.rs] - registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case SUBI:
			registers->regs[instr.rt] = registers->regs[instr.rs] - instr.imm;
			registers->state[instr.rt] = 1;
			break;
		case MUL:
			registers->regs[instr.rd] = registers->regs[instr.rs] * registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case MULI:
			registers->regs[instr.rt] = registers->regs[instr.rs] * instr.imm;
			registers->state[instr.rt] = 1;
			break;
		default:
			printf("Invalid opcode for selected Optype - %d\n", instr.code);
			break;
	}

	pc += 4;

	return 0;
}


/****************************************************************************
** Function: func_sim
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_logic(struct instruction instr, struct Registers *registers){
	switch (instr.code) {
		case OR:
			registers->regs[instr.rd] = registers->regs[instr.rs] | registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case ORI:
			registers->regs[instr.rt] = registers->regs[instr.rs] | instr.imm;
			registers->state[instr.rt] = 1;
			break;
		case AND:
			registers->regs[instr.rd] = registers->regs[instr.rs] & registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case ANDI:
			registers->regs[instr.rd] = registers->regs[instr.rs] & instr.imm;
			registers->state[instr.rt] = 1;
			break;
		case XOR:
			registers->regs[instr.rd] = registers->regs[instr.rs] ^ registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case XORI:
			registers->regs[instr.rd] = registers->regs[instr.rs] ^ instr.imm;
			registers->state[instr.rt] = 1;
			break;
		default:
			printf("Invalid opcode for selected Optype - %d\n", instr.code);
			break;
	}

	pc += 4;

	return 0;
}


/****************************************************************************
** Function: func_sim
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_mem(struct instruction instr, struct Memory *memory, struct Registers *registers){
	int addr;
	switch (instr.code) {
		case LDW:
			addr = (registers->regs[instr.rs] + instr.imm) / 4;
			registers->regs[instr.rt] = memory->mem[addr];
			registers->state[instr.rt] = 1;
			break;
		case STW:
			addr = (registers->regs[instr.rs] + instr.imm) / 4;
			memory->mem[addr] = registers->regs[instr.rt];
			memory->state[addr] = 1;
			break;
		default:
			printf("Invalid opcode for selected Optype - %d\n", instr.code);
			break;
	}

	pc += 4;

	return 0;
}


/****************************************************************************
** Function: func_sim
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_cntrl(struct instruction instr, struct Registers *registers){
	int new_pc = pc + 4;;

	switch (instr.code) {
		case BZ:
			if (registers->regs[instr.rs] == 0)
				new_pc = pc + ((instr.rt<<16)+instr.imm);
			break;
		case BEQ:
			if (registers->regs[instr.rs] == registers->regs[instr.rt])
				new_pc = pc + (instr.imm);
			break;
		case JR:
			new_pc = registers->regs[instr.rs];
			break;
		case HALT:
			halt = 1;
			break;
		default:
			printf("Invalid opcode for selected Optype - %d\n", instr.code);
			pc += 4;
			break;
	}

	pc = new_pc;

	return 0;
}




/*************************************************************************************************************
** Print Function
*************************************************************************************************************/
/****************************************************************************
** Function: print_regs
** Authors: Kai Roy
** Version: v1.0.0
** Description: Print the contents of the registers
****************************************************************************/
void print_regs(struct Registers *registers){
	printf("\nFinal Register State\n");
	for (int i = 0; i < MAX_REGS; i++) {
		if (registers->state[i] == 1)
			printf("R%d: %d\n", i, registers->regs[i]);
	}
}


/****************************************************************************
** Function: print_regs
** Authors: Kai Roy
** Version: v1.0.0
** Description: Print the contents of the memory
****************************************************************************/
void print_mem(struct Memory *memory){
	printf("\nFinal Memory State\n");
	for (int i = 0; i < LINE; i++) {
		if (memory->state[i] == 1)
			printf("Address: %d, Contents: %d\n", i*4, memory->mem[i]);
	}
}


//Functions from file_work.c--------------------
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



