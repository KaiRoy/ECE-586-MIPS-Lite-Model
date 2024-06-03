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
#include <stdbool.h>


/****************************************************************************
** Macros
****************************************************************************/
#define MAX_REGS 32
#define MAXLEN 10 //8 hex values on a given line, room for the null character '\0', and room for the newline character
#define LINECOUNTMAX 1024 //Only 1024 lines from the memory image can be read

#define PIPELINESIZE 5
#define RTYPE 99
#define ITYPE 100


/****************************************************************************
** Model Structure
****************************************************************************/
enum optype {ARITHMETIC, LOGIC, MEMORY, CONTROL};
enum opcode {
	ADD     = 0x00, 	//000000
    ADDI    = 0x01, 	//000001
    SUB    	= 0x02, 	//000010
    SUBI    = 0x03, 	//000011
    MUL     = 0x04, 	//000100
    MULI    = 0x05, 	//000101
    OR      = 0x06, 	//000110
    ORI     = 0x07, 	//000111
    AND     = 0x08, 	//001000
    ANDI    = 0x09, 	//001001
    XOR     = 0x0A, 	//001010
    XORI    = 0x0B, 	//001011
    LDW     = 0x0C, 	//001100
    STW     = 0x0D, 	//001101
    BZ      = 0x0E, 	//001110
    BEQ     = 0x0F, 	//001111
    JR      = 0x10, 	//010000
    HALT    = 0x11,  	//010001
    NNOP     = 0xFF
};

struct instruction {
	enum optype 	type;
	enum opcode 	code;
	unsigned int 	rs;
	unsigned int 	rt;
	unsigned int 	rd;
	signed int 		imm;
	bool 	RegDst;
	bool 	WriteReg;
	bool 	ReadReg1;
	bool 	ReadReg2;
};


// Memory Structures made by Kamal Smith
struct Memory{//Holds array of of lines of memory and the size
	signed int mem[LINECOUNTMAX];//This will have the current memory that we are working with
    int state[LINECOUNTMAX];
};

struct Registers{//the CPU registers as int array with 32 max registers
    signed int regs[MAX_REGS];
    int state[MAX_REGS];
};


/****************************************************************************
** Globals
****************************************************************************/
int instr_count = 0;
int arith_count = 0;
int logic_count = 0;
int mem_count = 0;
int cntrl_count = 0;

int mode;
int halt = 0;
int pc = 0;
int old_pc = 0;

int line_count; //made global so it's visible to the timing simulator

int mem[LINECOUNTMAX]; //array of 1024 integer values
const int Opcode_Mask = 0xFC000000;
const int Rs_Mask = 0x03E00000;
const int Rt_Mask = 0x001F0000;
const int Rd_Mask = 0x0000F800;
const int Immediate_Mask = 0x0000FFFF;

struct instruction instr_arr[LINECOUNTMAX];

struct instruction new_instruction;
struct instruction IF_stage;
struct instruction ID_stage;
struct instruction EXE_stage;
struct instruction MEM_stage;
struct instruction WB_stage;
struct instruction NOP;

//Timing Related
int clk_cnt = 0; //only one clock
//int clk_cnt_with_forwarding = 0;

int stalls_no_forwarding = 0;
int stalls_with_forwarding = 0;

int total_stalls_no_forwarding = 0;
int total_stalls_with_forwarding = 0;

int ID_EXE_flag;
int EXE_MEM_flag;


/****************************************************************************
** Function Prototypes
****************************************************************************/
// Memory/Trace File Handling
int containshex(char string[]); 	//checks for blank lines.
int Mem_Image_Handler(char line[]); //Converts a line in the image file to an integer and stores in the memory array
int TextToHex(char value); 			//converts a character to an integer value
void init(struct Memory *memory, struct Registers *registers);

// Functional Simulator
int func_sim(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_arith(struct instruction instr, struct Registers *registers);
int func_logic(struct instruction instr, struct Registers *registers);
int func_mem(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_cntrl(struct instruction instr, struct Registers *registers);

//Timing Simulator
int timing_sim(struct instruction instr, struct Memory *memory, struct Registers *registers);
void init_pipeline(struct instruction *IF, struct instruction *ID, struct instruction *EXE, struct instruction *MEM, struct instruction *WB, struct instruction *NOP);
int instruction_type(enum opcode OpCode); //returns the type of instruction. R type or I type

// User Interface
void menu();
void print_regs(struct Registers *registers);
void print_mem(struct Memory *memory);


/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	/*----- Initial -----*/
	//Initial Print
	printf("\nProgram Start\n\n");

	//Initialization
	struct Memory memory;
	struct Registers registers;
	pc = 0;

	char fileName[50];

	/*----- Memory Image File Handling -----*/
	//User Input here
	printf("\nEnter your Trace File: ");
	scanf("%s", fileName);

	FILE *fp;   //file pointer
	 fp = fopen(fileName, "r");				//User Input Name
//	fp = fopen("MemoryImage.txt", "r");		//Set Name

	if (fp == NULL)
	{
		printf("ERROR: Could not open file\n");
		exit(1);
	}

	//string to store a given trace file line
    char line[MAXLEN];
    line_count = 0;	//Keeps track of how many lines are read from the memory image file (max is 1024)
    int mem_index = 0; 	//index for memory, used instead of line_count because we want to count lines starting at 1 and not 0

    //Struct declaration
    struct instruction components;

    //Extract the string from the text file and place it into the memory array
    while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'	//Nick Allmeyer
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
    for (int i = 0; i < line_count; i++)	//Made by Nick Allmeyer, Jesus Zavala
    {
		components.code = (mem[i] & Opcode_Mask) >> 26;
		components.rs = (mem[i] & Rs_Mask) >> 21;
		components.rt = (mem[i] & Rt_Mask) >> 16;
		components.rd = (mem[i] & Rd_Mask) >> 11;
		components.imm = (mem[i] & Immediate_Mask);
        if (components.code == ADD ||
			components.code == ADDI ||
			components.code == SUBI ||
            components.code == SUB ||
            components.code == MUL ||
			components.code == MULI)
        {
			components.type = ARITHMETIC;
        }
        else if (components.code == OR ||
				 components.code == ORI ||
                 components.code == AND ||
                 components.code == ANDI ||
				 components.code == XOR ||
                 components.code == XORI)
        {
			components.type = LOGIC;
        }
		else if (components.code == LDW ||
                 components.code == STW)
        {
			components.type = MEMORY;
        }
		else if (components.code == BZ ||
                 components.code == BEQ ||
                 components.code == JR ||
                 components.code == HALT)
        {
			components.type = CONTROL;
        }
        else
        {
            // printf("ERROR: Invalid Opcode\n");
        }

		instr_arr[i] = components;
    }

	//Close File
	if (fclose(fp) == EOF)
	{
		printf("ERROR: File close not successful\n");
	}

	init(&memory, &registers);


	/*----- Simulator -----*/
	// Select Simulator Mode
	menu();

	// Run Instructions
	init_pipeline(&IF_stage, &ID_stage, &EXE_stage, &MEM_stage, &WB_stage, &NOP);
	struct instruction instr;
	int count = 0;
	while (!halt && (pc < 1024)) {
		instr = instr_arr[pc];
//		if (instr.rd == 3 || instr.rt == 3)
//			printf("R3 - Line %d\n", pc+1);
		func_sim(instr, &memory, &registers);
		// Insert Timing/Pipeline Simulator ----------------------------
		timing_sim(instr, &memory, &registers);
	}
	
// 	timing_sim(instr, &memory, &registers);


	/*----- Print Results -----*/
	printf("\n\nInstruction Counts\n");
	printf("Total number of instructions: %d\n", instr_count);
	printf("Arithmetic instructions: %d\n", arith_count);
	printf("Logical instructions: %d\n", logic_count);
	printf("Memory access instructions: %d\n", mem_count);
	printf("Control transfer instructions: %d\n", cntrl_count);

	printf("\nFinal Register State\n");
	printf("\nProgram Counter: %d\n", pc*4);
	print_regs(&registers);
	print_mem(&memory);

	if (mode == 1) {
		// Insert UI function for Timing (no forwarding) ----------------
		printf("\nClock Count = %d\n", clk_cnt);
		printf("Stall count no forwarding: %d\n", total_stalls_no_forwarding);
        printf("Total number of clock cycles no forwarding: %d\n", clk_cnt + total_stalls_no_forwarding);

        printf("Stall count with forwarding: %d\n", stalls_with_forwarding);
        printf("Total number of clock cycles with forwarding: %d\n", clk_cnt + stalls_with_forwarding);
	} else if (mode == 2) {
		// Insert UI function for Timing (forwarding) -------------------
	}


	printf("\nProgram Halted\n");
	return EXIT_SUCCESS;
}



/*************************************************************************************************************
** Memory/Trace File Handling Functions
*************************************************************************************************************/
/****************************************************************************
** Function: containshex
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Checks for blank lines in memory image
****************************************************************************/
int containshex(char string[])
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


/****************************************************************************
** Function: TextToHex
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Converts a character to an integer
****************************************************************************/
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


/****************************************************************************
** Function: Mem_Image_Handler
** Authors: Nick Allmeyer
** Version: v1.0.0
** Description: Function converts a single line of text from the memory image
** to an integer and stores that integer in a memory array
****************************************************************************/
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


/****************************************************************************
** Function: init
** Authors: Kamal Smith
** Version: v1.0.0
** Description:
****************************************************************************/
void init(struct Memory *memory, struct Registers *registers){
	for(int i = 0; i < LINECOUNTMAX; i++) {
		memory->mem[i] = mem[i];		// Will this be needed in the long run?
		memory->state[i] = 0;
	}

	for(int j = 0; j < MAX_REGS; j++) {
		registers->regs[j] = 0;
		registers->state[j] = 0;
	}

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

	instr_count++;

	return 0;
}


/****************************************************************************
** Function: func_arith
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
			printf("Invalid Opcode for selected Optype(A) - %d\n", instr.code);
			break;
	}

	arith_count++;
	pc++;

	return 0;
}


/****************************************************************************
** Function: func_logic
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
			registers->regs[instr.rt] = registers->regs[instr.rs] & instr.imm;
			registers->state[instr.rt] = 1;
			break;
		case XOR:
			registers->regs[instr.rd] = registers->regs[instr.rs] ^ registers->regs[instr.rt];
			registers->state[instr.rd] = 1;
			break;
		case XORI:
			registers->regs[instr.rt] = registers->regs[instr.rs] ^ instr.imm;
			registers->state[instr.rt] = 1;
			break;
		default:
			printf("Invalid Opcode for selected Optype(L) - %d\n", instr.code);
			break;
	}

	logic_count++;
	pc++;

	return 0;
}


/****************************************************************************
** Function: func_mem
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
			printf("Invalid Opcode for selected Optype(M) - %d\n", instr.code);
			break;
	}

	mem_count++;
	pc++;

	return 0;
}


/****************************************************************************
** Function: func_cntrl
** Authors: Kai Roy
** Version: v1.0.0
** Description:
****************************************************************************/
int func_cntrl(struct instruction instr, struct Registers *registers){
	int new_pc = pc + 1;

	switch (instr.code) {
		case BZ:
			if (registers->regs[instr.rs] == 0)
				new_pc = pc + (instr.imm);
			break;
		case BEQ:
			if (registers->regs[instr.rs] == registers->regs[instr.rt])
				new_pc = pc + (instr.imm);
			break;
		case JR:
			new_pc = (registers->regs[instr.rs])/4;
			break;
		case HALT:
			halt = 1;
			break;
		default:
			printf("Invalid Opcode for selected Optype(C) - %d\n", instr.code);
			pc++;
			break;
	}

//	new_pc = pc + 1;
	cntrl_count++;
	pc = new_pc;

	return 0;
}




/*************************************************************************************************************
** User Interface Functions
*************************************************************************************************************/
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


/****************************************************************************
** Function: print_regs
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Print the contents of the registers
****************************************************************************/
void print_regs(struct Registers *registers){
	// printf("\nFinal Register State\n");
	for (int i = 0; i < MAX_REGS; i++) {
		if (registers->state[i] == 1)
			printf("R%d: %d\n", i, registers->regs[i]);
	}
}


/****************************************************************************
** Function: print_regs
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Print the contents of the memory
****************************************************************************/
void print_mem(struct Memory *memory){
	printf("\nFinal Memory State\n");
	for (int i = 0; i < LINECOUNTMAX; i++) {
		if (memory->state[i] == 1)
			printf("Address: %d, Contents: %d\n", i*4, memory->mem[i]);
	}
}

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/*                            Timing Functions                             */
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

//Initialize stage structs
void init_pipeline(struct instruction *IF, struct instruction *ID, struct instruction *EXE, struct instruction *MEM, struct instruction *WB, struct instruction *NOP)
{
    //*(IF).OpCode same as IF->OpCode
    IF->code = 0xFF;
    IF->rs = 33; //impossible value for a register in mips
    IF->rt = 34;
    IF->rd = 35;
    IF->imm = 0;

    ID->code = 0xFF;
    ID->rs = 36; //impossible value for a register in mips
    ID->rt = 37;
    ID->rd = 38;
    ID->imm = 0;

    EXE->code = 0xFF;
    EXE->rs = 39; //impossible value for a register in mips
    EXE->rt = 40;
    EXE->rd = 41;
    EXE->imm = 0;

    MEM->code = 0xFF;
    MEM->rs = 42; //impossible value for a register in mips
    MEM->rt = 43;
    MEM->rd = 44;
    MEM->imm = 0;

    WB->code = 0xFF;
    WB->rs = 45; //impossible value for a register in mips
    WB->rt = 46;
    WB->rd = 47;
    WB->imm = 0;

    NOP->code = 0xFF;
    NOP->rs = 0; //impossible value for a register in mips
    NOP->rt = 0;
    NOP->rd = 0;
    NOP->imm = 0;
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

//Printing values in a struct
void print_struct(struct instruction stage)
{
    printf("Opcode: %x\n", stage.code);
    printf("Rs: R%d\n", stage.rs);
    printf("Rt: R%d\n", stage.rt);
    printf("Rd: R%d\n", stage.rd);
    printf("Imm: %d\n", stage.imm);
    printf("\n");
}

//timing simulator
int timing_sim(struct instruction instr, struct Memory *memory, struct Registers *registers)
{
	new_instruction = instr;

	//Shift instructions through pipeline. Order is important here
	WB_stage = MEM_stage; //MEM to WB
	MEM_stage = EXE_stage; //EXE to MEM
	EXE_stage = ID_stage; //ID to EXE
	ID_stage = IF_stage; //IF to ID
	IF_stage = new_instruction; //new instruction enters the pipeline

	clk_cnt++;

	if (MEM_stage.code == LDW, MEM_stage.code == STW) {
		total_stalls_no_forwarding +=1;
	}

	// stalls_no_forwarding = 0;
	if(IF_stage.code == HALT)
	{
		clk_cnt += 4;
		return 0;
	}
	
	//reset  the flags for hazard detection
	ID_EXE_flag = 0;
	EXE_MEM_flag = 0;

	if (pc != old_pc+1) {
		clk_cnt +=2 ;
		// goto branch;
	}

	old_pc = pc;

	hazard_detection: //checking ID stage compared to EXE and MEM
		if (ID_stage.code == NNOP) {
			return 0;
		}
		else if(instruction_type(ID_stage.code) == RTYPE )
		{
			if(1) //check exe stage
			{
				if(EXE_stage.code == NNOP){
					ID_EXE_flag = 0;
				}
				else if(instruction_type(EXE_stage.code) == RTYPE)
				{
					//compare exe destination with id source
					if (EXE_stage.rd == ID_stage.rs || EXE_stage.rd == ID_stage.rt)
					{
						ID_EXE_flag = 1;
					}
				}
				else //instruction_type(EXE_stage) == ITYPE
				{
					//compare exe destination with id source
					if (EXE_stage.rt == ID_stage.rs || EXE_stage.rt == ID_stage.rs)
					{
						ID_EXE_flag = 1;
					}
				}
			}
			if(1) //check mem stage
			{
				if(MEM_stage.code == NNOP) {
					EXE_MEM_flag = 0;
				}
				else if(instruction_type(MEM_stage.code) == RTYPE)
				{
					//compare mem destination with id source
					if (MEM_stage.rd == ID_stage.rs || MEM_stage.rd == ID_stage.rt)
					{
						EXE_MEM_flag = 1;
					}
				}
				else //instruction_type(mem_stage) == ITYPE
				{
					//compare mem destination with id source
					if (MEM_stage.rt == ID_stage.rs || MEM_stage.rt == ID_stage.rs)
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
				if(EXE_stage.code == NNOP){
					ID_EXE_flag = 0;
				}
				else if(instruction_type(EXE_stage.code) == RTYPE)
				{
					//compare exe destination with id source
					if (EXE_stage.rd == ID_stage.rs)
					{
						ID_EXE_flag = 1;
					}
				}
				else //instruction_type(EXE_stage) == ITYPE
				{
					//compare exe destination with id source
					if (EXE_stage.rt == ID_stage.rs)
					{
						ID_EXE_flag = 1;
					}
				}
			}
			if(1) //check mem stage
			{
				if(MEM_stage.code == NNOP) {
					EXE_MEM_flag = 0;
				}
				else if(instruction_type(MEM_stage.code) == RTYPE)
				{
					//compare mem destination with id source
					if (MEM_stage.rd == ID_stage.rs)
					{
						EXE_MEM_flag = 1;
					}
				}
				else //instruction_type(mem_stage) == ITYPE
				{
					//compare mem destination with id source
					if (MEM_stage.rt == ID_stage.rs)
					{
						EXE_MEM_flag = 1;
					}
				}
			}
		}

		//reset stall amount
		stalls_no_forwarding = 0;

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
			// printf("No Hazard\n");
			return 0;
		}

		//calculate total number of stalls by the program
		total_stalls_no_forwarding += stalls_no_forwarding;

		//insert NOPS into EX stage
		for (int i = 0; i < stalls_no_forwarding; i++)
		{
			//clk_cnt++;

			IF_stage = IF_stage;
			ID_stage = ID_stage;
			EXE_stage = NOP;
			WB_stage = MEM_stage;
			MEM_stage = EXE_stage;
		}
		// goto normal;


	// branch: //branch
	// 	{
	// 		clk_cnt+=2;
	// 	}

    return 0;
}
