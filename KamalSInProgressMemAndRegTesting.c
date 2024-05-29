/******************************************************************************
Kamal Smith
ECE486 Final Project Memory and register work
Code for memory access instructions in the project. Anyone in my group is allowed
to modify this code if needed.

FROM PROJECT SPECS
c. Memory Access Instructions
    i. LDW Rt Rs Imm (Add the contents of Rs and the immediate value “Imm” to generate
    the effective address “A”, load the contents (32-bits) of the memory location at address
    “A” into register Rt). Opcode: 001100
    ii. STW Rt Rs Imm (Add the contents of Rs and the immediate value “Imm” to generate
    the effective address “A”, store the contents of register Rt (32-bits) at the memory
    address “A”). Opcode: 001101

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SIZE 4096
#define MAX_REGS 32

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE486
 * These are the two structs that I used for constructing the Memory
 * struct "Memory" contains a char pointer "current_mem" with an array size of 
 * 4096 and struct "Registers" that contains an int "regs" with an array size of 32
 * 
 * @defines
 * #define SIZE 4096
 * #define MAX_REGS 32
*/
struct Memory{//Holds array of of lines of memory and the size
    char *current_mem[SIZE];//This will have the current memory that we are working with
};

struct Registers{//the CPU registers as int array with 32 max registers
    int regs[MAX_REGS];
};


/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void init_mem(struct Memory *memory, const char *path);
 * 
 * Function to initalize the memory in the program and read a line
 * If there is no error in the file read, will continue to get lines.
 * If there is an error while getting lines, will print a message, close the 
 * file and exit the program.
 * If program opens and allocates memory without issues, will copy the contents
 * of line into the current_mem index.
 * 
 * NOTE: May be better to break this into two different functions, can rewrite
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * constant char with a pointer "path"
*******************************************************************/
void init_mem(struct Memory *memory, const char *path){
    FILE *file = fopen(path,"r");//opening the txt file with the memory in read mode
    if(!file){//File does not open properly
        printf("Error in init memory function opening the file");
        exit(-1);
    }else{//File opens so we set up a way to track where we are in memory
        int i = 0; //Where we are in the current memory
        char line[SIZE];//Storing the lines that we read from mem
        while(fgets(line, sizeof(line), file)){
            line[strcspn(line,"\n")] = 0;//Will I be missing first line here?
            memory -> current_mem[i] = (char *)malloc(strlen(line) + 1);
            if (memory -> current_mem[i] == NULL){ //Checking if error with memory allocation
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

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * char* read_word(struct Memory *memory, int i);
 * 
 * Will return a string at the location of current_mem
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * int "i" used as the indexer
*******************************************************************/
char* read_word(struct Memory *memory, int i){
    return memory->current_mem[i];
}


/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void write_word(struct Memory *memory, int i, const char *word);
 * 
 * Allocates memory for a word and checks if theres an error with the allocation
 * if no error, will copy the contents of pointer word into the current index of
 * current_mem. edited from "init_mem" function.
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * int "i" used as the indexer
 * constant character pointer "word"
*******************************************************************/
void write_word(struct Memory *memory, int i, const char *word){
    memory->current_mem[i] = (char *)malloc(strlen(word) + 1);
    if (memory -> current_mem[i] == NULL){ //Checking if error with memory allocation
        printf("Memory allocation error in write_word function");
    }else{
        strcpy(memory -> current_mem[i], word);//copy contents of ptr word into memory
    }
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * int byte_read(struct Memory *memory, int i);
 * 
 * Calculates the byte offset of the word from memory and converts from hex to 
 * a simpler int
 * Will return the value of byte
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * int "i" used as the indexer
*******************************************************************/
int byte_read(struct Memory *memory, int i){
    int offset = (i % 4);
    char *word = memory -> current_mem[i - offset];
    uint32_t valueWord = strtol(word, NULL, 16);
    int byte = (valueWord >> ((3 - offset) * 8)) & 0xFF;
    return byte;
    
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void byte_write(struct Memory *memory, int i, const char *NewWord);
 * 
 * Calculates the byte offset of the word from memory and converts from hex to 
 * a simpler int. "valueWord" is then cleared and set with a new value then wrote
 * back as a hex value
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * int "i" used as the indexer
 * constant character pointer "NewWord"
*******************************************************************/
void byte_write(struct Memory *memory, int i, const char *NewWord){
    int offset = (i % 4);
    NewWord = memory -> current_mem[i - offset];
    uint32_t valueWord = strtol(NewWord, NULL, 16);
    uint32_t valueByte = strtol(NewWord, NULL ,16);
    
    valueWord &= ~(0xFF << ((3 - offset) * 8)); // clearing offset using bitwise AND 
    valueWord |= (valueByte << ((3 - offset) * 8));//Setting new value using bitwise OR
    
    sprintf(memory -> current_mem[i - offset], "%08X", valueWord);//Writes back the converted word
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void init_regs(struct Registers *registers);
 * 
 * Initalizes all 32 of the registers and sets them to zero
 * 
 * @parameters
 * struct "Registers" with a pointer "register"
*******************************************************************/
void init_regs(struct Registers *registers){
    for(int i = 0; i < MAX_REGS; i++){
        registers->regs[i] = 0;
    }
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * int reg_read(struct Registers *registers, int i);
 * 
 * Returns the value of the register at the indexed location
 * 
 * @parameters
 * struct "Registers" with a pointer "registers"
 * int i used for indexing
 * 
*******************************************************************/
int reg_read(struct Registers *registers, int i){
    return registers -> regs[i];
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void reg_write(struct Registers *registers, int i, int num_word)
 * 
 * Sets regs at "i" index to the value of num_word
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
 * int "i" for indedxing
 * int "num_word" as a buffer to place values into regs
*******************************************************************/
//puts the value of the reg at the location i
void reg_write(struct Registers *registers, int i, int num_word){
    registers-> regs[i] = num_word;
}

/****************************************************************
 * Kamal Smith (kamal@pdx.edu) ECE 486
 * 
 * void mem_free(struct Memory *memory);
 * 
 * Every malloc needs a free, cleans up all that allocated memory
 * 
 * @parameters
 * struct "Memory" with a pointer "memory"
*******************************************************************/
void mem_free(struct Memory *memory){
    for(int i = 0; i < SIZE; i += 4){
        if(memory->current_mem[i] != NULL){
        free(memory->current_mem[i]);
        memory -> current_mem[i] = NULL;
        }
    }
}

int main()
{
    struct Memory memory = {0};
    struct Registers registers;
    
    //BELOW IS WHERE WE ADD THE TXT FILE
    init_mem(&memory,"MEM_FILE_HERE.txt");
    init_regs(&registers);
    
    //THE STUFF BELOW HERE IS FOR TESTING
    
    return 0;
}
