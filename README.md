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
- **Pipeline simulator**: Keeps track of the current clock cycle, the instruction in each pipeline stage, identifies the stalls and hazards and propagates instructions from one pipeline stage to the next.

We were supplied with OP codes that we needed to use so our program could understand the provided instructions. This included: 

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

<!-- Insert Image of Format? -->

**I-Type**

<!-- Insert Image of Format? -->

**J-Type**

<!-- Insert Image of Format? -->


## Designs

<!-- Create a Block Diagram of the System?-->
<!-- ![System Block Diagram](Assets/ECE%2044x%20Block%20Diagram.png) -->

<!-- Reiterate project requirement document -->
<!-- Necessary Commands, code structure, etc-->

<!-- Design Implementation Choices-->



## Current State of the Project

<!-- Outputs from the test files match the outputs given in class -->


## Post Project Notes

<!-- Comment on timing organization, we were not able to get the project done before the demo, but were able to before the end of the term -->


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

