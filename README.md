# ECE-586-MIPS-Lite-Model
The focus of this project was to create a MIPS Lite Model for the ECE 586 - Computer Architecture Class

<!-- Insert Image Here -->

<!-- LINK SUMMARY VIDEO SOMEWHERE IN THE README!! -->

## Table of Contents

- [Description](#description)
- [Design](#design)
- [Current State](#current-state-of-the-project)
- [Post Project Notes](#post-project-notes)
- [Installation](#installation) <!-- Should I have Installation and Usage above or below the Design, Current State, Changes, etc? -->
- [Usage](#usage)
- [Credits](#credits)
- [License](#license)

## Description

In this project, we created a simulated version of the MIPS ISA called “MIPS lite” which included a 5-stage pipeline with both non-forwarding and forwarding modes. 
Our MIPS lite was 32-bits and we implemented the following types of components:
 
- **Trace reader**: This reads the memory image and sends the next instruction to the decoder.
- **Instruction decoder**: Decodes the instruction, and determines what registers it will be sent to.
- **Functional Simulator**: Simulates the instructions behaviors while keeping track of the register and memory states. 
- **Pipeline/Timing simulator**: Keeps track of the current clock cycle, the instruction in each pipeline stage, identifies the stalls and hazards and propagates instructions from one pipeline stage to the next.

<!-- Instruction Formatting -->
The following are the instructions needed for our MIPS Lite Model and their corresponding opcodes: 
**Arithmetic:**
- ADD	= 0x00 	//000000
- ADDI	= 0x01 	//000001
- SUB	= 0x02 	//000010
- SUBI	= 0x03 	//000011
- MUL	= 0x04 	//000100
- MULI	= 0x05 	//000101

**Logical:**
- OR	= 0x06	//000110
- ORI	= 0x07 	//000111
- AND	= 0x08 	//001000
- ANDI	= 0x09 	//001001
- XOR	= 0x0A 	//001010
- XORI	= 0x0B 	//001011

**Memory:**
- LDW	= 0x0C 	//001100
- STW	= 0x0D 	//001101

**Control:**
- BZ	= 0x0E 	//001110
- BEQ	= 0x0F 	//001111
- JR	= 0x10 	//010000
- HALT	= 0x11  //010001
- NNOP	= 0xFF  //111111

The three instruction formats that our MIPS Lite Model supports include:

**R-Type**

| Opcode[31:26] | Rs[25:21] | Rt[20:16] | Rd[15:11] | x[10:0] |
| --- | --- | --- | --- | --- |

**I-Type**

| Opcode[31:26] | Rs[25:21] | Rt[20:16] | imm[15:0] |
| --- | --- | --- | ------ |

**J-Type**

| Opcode[31:26] | Rs[25:21] | x[20:0] |
| --- | --- | --------- |


<!-- Pipeline structure -->
The MIPS Lite model must implement a 5-stage pipeline for timing simulator

- **Instruction Fetch**: Fetch the instruction, increment the program counter.
- **Instruction Decode**: takes the instruction and breaks it down into something it can understand while also checking contents of the source registers.
- **Execute**: Executes the operation that was decoded.
- **Memory Access**: Loads and stores any instructions if needed.
- **Write Back**: Write the result back to the destination register.

<!-- Modes -->
The model needs to have three modes, that can be run without recompilation.
- The first mode is purely functional with no timing simulation.
- The second mode implements timing simulation for the 5-stage pipeline without any forwarding. 
- The second mode implements timing simulator for the 5-stage pipeline with forwarding enabled. 

<!-- Output Format -->
The program must have the following output format:

<!-- Insert Image of Format? -->


## Designs

<!-- Create a Block Diagram of the System?-->
<!-- ![System Block Diagram](Assets/ECE%2044x%20Block%20Diagram.png) -->

<!-- Design Implementation Choices-->
We first created a struct for the instructions to contain the opcode, type of instruction, registers, and immediate values. During the handling of the trace file/memory image, an array is created to hold every instruction in the memory image. A PC variable is used to index into this array during functional simulation. The program/data memory and the register files are modeled as simple arrays. 

The model simulation is split into two main functions: the functional simulator and the timing simulator. For simplicity, these functions were designed in such a way that no direct interaction is needed for either to function as intended. Both functions require the current instruction, memory array, and register array as inputs. 

The functional simulator is further split into four functions, each handling a different type of instruction (Arithmetic, Logic, Memory, Control). It was decided to split into this way instead of splitting based on opcode format i.e. R type, I type, J type, was the requirement to keep track of how many of each type of instruction being run. 

The timing simulator handles both timing modes with most of the calculations being the same with only a couple of differences. The timing simulator contains a five element array to keep track of which instruction is which stage of the pipeline. By comparing the regsiters being used by each instruction, specific data hazards can be found and depending on the hazard the pipeline is stalled by that amount. Stalls do two things in the timing simulator, they add additional clock cycles to the ongoing clock counter and the add nop instructions into the pipeline equal to the stalls needed. This is necessary to ensure that no additional hazards are considered that would be resolved from a previous hazards stall. The number of stalls per hazard and the nop insertion is where the forwarding and non-forwarding modes have differences. 


## Current State of the Project

By the end of the project, our model was able to match the example output with the memory image provided for testing. With our design being incomplete during our demo time, we do not know how the model would handle that memory image since we were not allowed to save the file. 

## Post Project Notes

The main issue faced with this project was time. We had delayed the beginning of this project for too long and while the model was finished by the end of the project, we were not able to have the program ready by our alotted demo time. 

While the model is fully functional, to our knowledge, the main improvements that can be made are optimizations to the code base. 


## Installation

<!-- Specify what the primary c file is -->
<!-- Potential reorganize repo and divert the other files into an archive folder -->



<!--
## Usage

Provide instructions and examples for use. Include screenshots as needed.

To add a screenshot, create an `assets/images` folder in your repository and upload your screenshot to it. Then, using the relative filepath, add it to your README using the following syntax:

    ```md
    ![alt text](assets/images/screenshot.png)
    ```

## Features

If your project has a lot of features, list them here.

## Tests

-->

## Credits

<!-- List your collaborators, if any, with links to their GitHub profiles. -->
- Kai Roy
- Nick Allmeyer
- Kamal Smith
- Jesus Zavala
- Daisy Perez

