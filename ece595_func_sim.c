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

#include <stdint.h>
#include <stdbool.h>


/*************************************************************************************************************
** Macros
*************************************************************************************************************/
#define SIZE 4096
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
	bool 	RegDst;
	bool 	WriteReg;
	bool 	ReadReg1;
	bool 	ReadReg2;
};

struct Memory{//Holds array of of lines of memory and the size
    char *current_mem[SIZE];//This will have the current memory that we are working with
};

struct Registers{//the CPU registers as int array with 32 max registers
    int regs[MAX_REGS];
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


/*************************************************************************************************************
** Function Prototypes
*************************************************************************************************************/
void menu();
int func_sim(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_arith(struct instruction instr, struct Registers *registers);
int func_logic(struct instruction instr, struct Registers *registers);
int func_mem(struct instruction instr, struct Memory *memory, struct Registers *registers);
int func_cntrl(struct instruction instr, struct Registers *registers);


void init_mem(struct Memory *memory, const char *path);
char* read_word(struct Memory *memory, int i);
void write_word(struct Memory *memory, int i, const char *word);
int byte_read(struct Memory *memory, int i);
void byte_write(struct Memory *memory, int i, const char *NewWord);
void mem_free(struct Memory *memory);

void init_regs(struct Registers *registers);
int reg_read(struct Registers *registers, int i);
void reg_write(struct Registers *registers, int i, int num_word);

void print_regs(struct Registers *registers);
void print_mem(struct Memory *memory);


/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	struct Memory memory = {0};
	struct Registers registers;

	//User Input for Memory
	char memFile[50];
	printf("\nEnter your Memory File: ");
	scanf("%s", memFile);

	// MEM TESTING
	// Initializations
	init_mem(&memory, memFile);
	init_regs(&registers);

	// Print contents
	print_regs(&registers);
	print_mem(&memory);

//	// FUNC SIM TESTING
//	char traceFile[50];
//
//	//User Input for Trace
//	printf("\nEnter your Trace File: ");
//	scanf("%s", traceFile);
//
//	// Testing Instructions
//	struct instruction instr1;
//	struct instruction instr2;
//	struct instruction instr3;
//
//	//Functional Simulator
//	func_sim(instr1, &memory, &registers);
//	func_sim(instr2, &memory, &registers);
//	func_sim(instr3, &memory, &registers);

	mem_free(&memory);

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
			printf("Optype does not match\n");
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

			break;
		case ADDI:

			break;
		case SUB:

			break;
		case SUBI:

			break;
		case MUL:

			break;
		case MULI:

			break;
		default:
			printf("Invalid Opcode for selected Optype\n");
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
int func_logic(struct instruction instr, struct Registers *registers){
	switch (instr.code) {
		case OR:

			break;
		case ORI:

			break;
		case AND:

			break;
		case ANDI:

			break;
		case XOR:

			break;
		case XORI:

			break;
		default:
			printf("Invalid Opcode for selected Optype\n");
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
int func_mem(struct instruction instr, struct Memory *memory, struct Registers *registers){
	switch (instr.code) {
		case LDW:

			break;
		case STW:

			break;
		default:
			printf("Invalid Opcode for selected Optype\n");
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
int func_cntrl(struct instruction instr, struct Registers *registers){
	switch (instr.code) {
		case BZ:

			break;
		case BEQ:

			break;
		case JR:

			break;
		case HALT:

			break;
		default:
			printf("Invalid Opcode for selected Optype\n");
			break;
	}
	return 0;
}




/*************************************************************************************************************
** Memory Function
*************************************************************************************************************/
/****************************************************************************
FROM PROJECT SPECS
c. Memory Access Instructions
    i. LDW Rt Rs Imm (Add the contents of Rs and the immediate value “Imm” to generate
    the effective address “A”, load the contents (32-bits) of the memory location at address
    “A” into register Rt). Opcode: 001100
    ii. STW Rt Rs Imm (Add the contents of Rs and the immediate value “Imm” to generate
    the effective address “A”, store the contents of register Rt (32-bits) at the memory
    address “A”). Opcode: 001101
****************************************************************************/
/****************************************************************************
** Function: init_mem
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Function to initialize the memory in the program and read a line
 * If there is no error in the file read, will continue to get lines.
 * If there is an error while getting lines, will print a message, close the
 * file and exit the program.
 * If program opens and allocates memory without issues, will copy the contents
 * of line into the current_mem index.
 *
 * NOTE: May be better to break this into two different functions, can rewrite
****************************************************************************/
void init_mem(struct Memory *memory, const char *path){
	// Opening the file with the memory in read mode
	FILE *file = fopen(path,"r");

    if (!file) {		//File does not open properly
        printf("Error in init memory function opening the file");
        exit(-1);
    } else {			//File opens
        // Set up a way to track where we are in memory
    	int i = 0; 			// Where we are in the current memory
        char line[SIZE];	// Storing the lines that we read from mem
        while(fgets(line, sizeof(line), file)){
            line[strcspn(line,"\n")] = 0;				//Will I be missing first line here?
            memory -> current_mem[i] = (char *)malloc(strlen(line) + 1);
            if (memory -> current_mem[i] == NULL){ 		//Checking if error with memory allocation
                printf("Memory allocation error in init_mem function");
                fclose(file);
                exit(1);
            }
            strcpy(memory -> current_mem[i], line);
            i += 4; //

        }

        fclose(file);
    }
}


/****************************************************************************
** Function: read_word
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Will return a string at the location of current_mem
****************************************************************************/
char* read_word(struct Memory *memory, int i){
    return memory->current_mem[i];
}


/****************************************************************************
** Function: write_word
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Allocates memory for a word and checks if theres an error with the allocation
 * if no error, will copy the contents of pointer word into the current index of
 * current_mem. edited from "init_mem" function.
****************************************************************************/
void write_word(struct Memory *memory, int i, const char *word){
    memory->current_mem[i] = (char *)malloc(strlen(word) + 1);
    if (memory -> current_mem[i] == NULL){ //Checking if error with memory allocation
        printf("Memory allocation error in write_word function");
    } else {
        strcpy(memory -> current_mem[i], word);//copy contents of ptr word into memory
    }
}


/****************************************************************************
** Function: byte_read
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Calculates the byte offset of the word from memory and converts from hex to
 * a simpler int
 * Will return the value of byte
****************************************************************************/
int byte_read(struct Memory *memory, int i){
    int offset = (i % 4);
    char *word = memory -> current_mem[i - offset];
    uint32_t valueWord = strtol(word, NULL, 16);
    int byte = (valueWord >> ((3 - offset) * 8)) & 0xFF;
    return byte;

}


/****************************************************************************
** Function: byte_write
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Calculates the byte offset of the word from memory and converts from hex to
 * a simpler int. "valueWord" is then cleared and set with a new value then wrote
 * back as a hex value
****************************************************************************/
void byte_write(struct Memory *memory, int i, const char *NewWord){
    int offset = (i % 4);
    NewWord = memory -> current_mem[i - offset];
    uint32_t valueWord = strtol(NewWord, NULL, 16);
    uint32_t valueByte = strtol(NewWord, NULL ,16);

    valueWord &= ~(0xFF << ((3 - offset) * 8)); // clearing offset using bitwise AND
    valueWord |= (valueByte << ((3 - offset) * 8));//Setting new value using bitwise OR

    sprintf(memory -> current_mem[i - offset], "%08X", valueWord);//Writes back the converted word
}


/****************************************************************************
** Function: mem_free
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Every malloc needs a free, cleans up all that allocated memory
****************************************************************************/
void mem_free(struct Memory *memory){
    for(int i = 0; i < SIZE; i += 4){
        if(memory->current_mem[i] != NULL){
        free(memory->current_mem[i]);
        memory -> current_mem[i] = NULL;
        }
    }
}




/*************************************************************************************************************
** Register Function
*************************************************************************************************************/
/****************************************************************************
** Function: init_regs
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Initalizes all 32 of the registers and sets them to zero
****************************************************************************/
void init_regs(struct Registers *registers){
    for(int i = 0; i < MAX_REGS; i++){
        registers->regs[i] = 0;
    }
}


/****************************************************************************
** Function: reg_read
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Returns the value of the register at the indexed location
****************************************************************************/
int reg_read(struct Registers *registers, int i){
    return registers -> regs[i];
}


/****************************************************************************
** Function: reg_write
** Authors: Kamal Smith
** Version: v1.0.0
** Description: Sets regs at "i" index to the value of num_word
** puts the value of the reg at the location i
****************************************************************************/
void reg_write(struct Registers *registers, int i, int num_word){
    registers-> regs[i] = num_word;
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
	for (int i = 0; i < MAX_REGS; i++) {
		printf("R%d = %d\n", i, registers->regs[i]);
	}
}


/****************************************************************************
** Function: print_regs
** Authors: Kai Roy
** Version: v1.0.0
** Description: Print the contents of the memory
****************************************************************************/
void print_mem(struct Memory *memory){
	for (int i = 0; i < SIZE; i+=4) {
		printf("\n!!!Not Implemented Yet!!!\n");
	}
}



