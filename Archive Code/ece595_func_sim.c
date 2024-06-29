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
	registers->regs[8] = 65;

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
			printf("Invalid Opcode for selected Optype - %d\n", instr.code);
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
			printf("Invalid Opcode for selected Optype - %d\n", instr.code);
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
			printf("Invalid Opcode for selected Optype - %d\n", instr.code);
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
			printf("Invalid Opcode for selected Optype - %d\n", instr.code);
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




/*************************************************************************************************************
** Testing Instructions
*************************************************************************************************************/
// Arithmetic
//	struct instruction instr1;
//	instr1.type = ARITHMETIC;
//	instr1.code = ADDI;
//	instr1.rs = 0;
//	instr1.rt = 1;
//	instr1.imm = 1000;
//	struct instruction instr2;
//	instr2.type = ARITHMETIC;
//	instr2.code = ADDI;
//	instr2.rs = 0;
//	instr2.rt = 2;
//	instr2.imm = 1200;
//	struct instruction instr3;
//	instr3.type = ARITHMETIC;
//	instr3.code = MULI;
//	instr3.rs = 1;
//	instr3.rt = 7;
//	instr3.imm = 3;
//	struct instruction instr4;
//	instr4.type = ARITHMETIC;
//	instr4.code = MUL;
//	instr4.rs = 1;
//	instr4.rt = 2;
//	instr4.rd = 8;
//	struct instruction instr5;
//	instr5.type = ARITHMETIC;
//	instr5.code = ADDI;
//	instr5.rs = 11;
//	instr5.rt = 0;
//	instr5.imm = 50;
//	struct instruction instr6;
//	instr6.type = ARITHMETIC;
//	instr6.code = SUB;
//	instr6.rs = 2;
//	instr6.rt = 0;
//	instr6.rd = 15;
//	struct instruction instr7;
//	instr7.type = ARITHMETIC;
//	instr7.code = SUBI;
//	instr7.rs = 2;
//	instr7.rt = 20;
//	instr7.imm = -12;

// OTHER
//	struct instruction instr1;
//	instr1.type = LOGIC;
//	instr1.code = OR;
//	instr1.rs = 7;
//	instr1.rt = 8;
//	instr1.rd = 9;
//	struct instruction instr2;
//	instr2.type = LOGIC;
//	instr2.code = AND;
//	instr2.rs = 7;
//	instr2.rt = 8;
//	instr2.rd = 10;
//	struct instruction instr3;
//	instr3.type = LOGIC;
//	instr3.code = XOR;
//	instr3.rs = 7;
//	instr3.rt = 8;
//	instr3.rd = 11;
//	struct instruction instr4;
//	instr4.type = MEMORY;
//	instr4.code = STW;
//	instr4.rs = 7;
//	instr4.rt = 8;
//	instr4.imm = 0;
//	struct instruction instr5;
//	instr5.type = MEMORY;
//	instr5.code = LDW;
//	instr5.rs = 7;
//	instr5.rt = 20;
//	instr5.imm = 50;
//	struct instruction instr6;
//	instr6.type = CONTROL;
//	instr6.code = BZ;
//	instr6.rs = 0;
//	instr6.rt = 9;
//	instr6.imm = 0;
//	struct instruction instr7;
//	instr7.type = CONTROL;
//	instr7.code = JR;
//	instr7.rs = 7;
//	instr7.rt = 0;
//	instr7.imm = 9;

