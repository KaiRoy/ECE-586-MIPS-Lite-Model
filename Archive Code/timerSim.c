#include <stdio.h>
#include <stdlib.h>

#define REG_NUM 32
#define STAGES 5

/********************************************************************************** 
										STRUCTS 
************************************************************************************/
//copied from func_sim.c
struct instruction {
	enum optype 	type;
	enum opcode 	code;
	unsigned int 	rs;
	unsigned int 	rt;
	unsigned int 	rd;
	signed int 		imm;
	bool			isNOP;
};

// This will define the stage that the pipeline is in
struct pipelineStage{
	instruction curr;
	bool occupied;
};
struct Memory{//Holds array of of lines of memory and the size
	signed int mem[LINE];//This will have the current memory that we are working with
    int state[LINE];
};

struct Registers{//the CPU registers as int array with 32 max registers
    signed int regs[MAX_REGS];
    int state[MAX_REGS];
};

/********************************************************************************** 
								GLOBAL VARS
************************************************************************************/

int timer = 0;


int registers[REG_NUM];
/********************************************************************************** 
							  FUNC PROTOTYPES
************************************************************************************/
bool fwd(PipelineStage *curr);
void stallHandle(PipelineStage *stage);
bool RAW();

/********************************************************************************** 
							  		MAIN
************************************************************************************/
int main(){

	while(1){
		//while loop will only be stopped once a HALT command is recieved
		if (RAW()){
			if(fwd(stage)){
			
			}
			else{
				stallHandle(stage);
				return;
			}
		}
		//pretend this is the HALT command interpreted via function made by other member
		if(HALT){
			break;
		}
		timer++;
	
	}
		

}


//This will check if the needed data is available from a subsequent pipeline stage 
//(EX, MEM, or WB). If so, fwding is applied

bool fwd(PipelineStage *curr){
	instruction currentInstr = currentStage ->instruction;
	
	for(int i = 1; i<NUM_STAGES; i++){
		if(!pipeline[i].occupied){
			continue;
		}
		
		instruction instrInPipeline = pipeline[i].instruction;
		
		if (currInstr.rs == instrInPipeline.rd || currInstr.rt == instrInPipeline.rd ){
			if(i ==2 || i ==3 || i ==4){
				return true;
			}
		}
	}
	
	return false;
	
}

void stallHandle(PipelineStage *stage){
	for(int i = NUM_STAGES -1; i>1; i--){
		pipeline[i]=pipeline[i-1];
	}
	
	pipeline[1].instruction.isNOP=true;
	pipeline[1].occupied=false;
	timer ++;
}

//function that checks for RAW hazards
bool RAW()
