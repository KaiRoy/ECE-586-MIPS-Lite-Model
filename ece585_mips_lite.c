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


/****************************************************************************
** Macros
****************************************************************************/
#define REGCOUNT 32


/****************************************************************************
** Model Structure
****************************************************************************/
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

};


/****************************************************************************
** Globals
****************************************************************************/
int instr_count = 0;
int arith_count = 0;
int logic_count = 0;
int mem_count = 0;
int cntrl_count = 0;




/****************************************************************************
** Function Prototypes
****************************************************************************/



/****************************************************************************
** Function: Main
** Version: v1.0.0
** Description: C Main Function
****************************************************************************/
int main(void) {
	clearCache();
	struct address addr;
	int step = 0;
	int state = 0;

	char fileName[50];
	int choice;

	//User Input here
	printf("\nEnter your Trace File: ");
	scanf("%s", fileName);

	FILE *fp;   //file pointer
	fp = fopen(fileName, "r");
	if (fp == NULL)
	{
		printf("ERROR: Could not open file\n");
		exit(1);
	}


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

	//string to store a given trace file line
	char line[MAXLEN];

	while (fgets(line, MAXLEN, fp) != NULL) //fgets terminates string line with '\0'
	{
		line[strcspn(line, "\n")] = 0; //remove newline character
		if (!containshex(line)) //ignore any blank lines
		{
			continue;
		}
//		step++;
		strcpy(addr.hex, line+2);

		//Verifying TraceData
		TraceDataHandler(line);

		// Run Instruction
		instruction = instr_decode(line);
		func_sim(instruction);
		switch (mode) {
			case 0: break;
			case 1: 
				pipe_sim(instruction, FALSE);
				break;
			case 2:
				pipe_sim(instruction, TRUE);
		}
	}

	//Close File
	if (fclose(fp) == EOF)
	{
		printf("ERROR: File close not successful\n");
	}

//	output_func();

	return EXIT_SUCCESS;
}


/****************************************************************************
** Function: initCache
** Authors: Kai Roy
** Version: v1.0.0
** Description: Initializes all valid bits in cache to 0.
****************************************************************************/
