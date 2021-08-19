#include "computer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char* name;

	// Function of corresponding opcode
	// Opocde takes current_microcode as argument and do different things based on it
	void (*operation)(uint8_t opcode,uint8_t current_microcode);

}InstructionList;

static Cpu cpu;
uint8_t ram[16];

//instuctions
void NOP(uint8_t opcode,uint8_t current_microcode)
{
	return;
}

void LDA(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.MAR = opcode & 0x0f;
			break;
		case 3:
			cpu.A = ram[cpu.MAR];
			break;
		default:
			break;
	}
	return;
}

void ADD(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.MAR = opcode & 0x0f;
			break;
		case 3:
			cpu.B = ram[cpu.MAR];
			cpu.SUM = cpu.A + cpu.B;
			
			break;
		case 4:
			cpu.A = cpu.SUM;
			// carry check
			cpu.CY = ((cpu.A + cpu.B) > 255) ? 1:0;

			//zero check
			cpu.ZR = (cpu.SUM) ? 0:1;
			break;
		default:
			break;
	}
	return;
}

void SUB(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.MAR = opcode & 0x0f;
			break;
		case 3:
			cpu.B = ram[cpu.MAR];
			cpu.SUM = cpu.A - cpu.B;

			break;
		case 4:
			/* Ben eater's computer add the two's compliement of B with A for subtraction.*/
			// carry check
			uint8_t two_comp_of_B = ~cpu.B + 1;
			cpu.CY = ((two_comp_of_B + cpu.A) > 255);

			cpu.A = cpu.SUM;
			
			//zero check
			cpu.ZR = (cpu.SUM) ? 0:1;
			
			break;
		default:
			break;
	}
	return;
}

void STA(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.MAR = opcode & 0x0f;
			break;
		case 3:
			ram[cpu.MAR] = cpu.A;
			break;
		default:
			break;
	}
	return;
}

void LDI(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.A = opcode & 0x0f;
			break;
		default:
			break;
	}
	return;
}

void JMP(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.PC = opcode & 0x0f;
			break;
		default:
			break;
	}
	return;
}

void JC(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			if(cpu.CY)
				cpu.PC = opcode & 0x0f;
			break;
		default:
			break;
	}
	return;
}

void JZ(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			if(cpu.ZR)
				cpu.PC = opcode & 0x0f;
			break;
		default:
			break;
	}
	return;
}

void OUT(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.OUT = cpu.A;
			break;
		default:
			break;
	}
	return;
}

void HLT(uint8_t opcode,uint8_t current_microcode)
{
	switch (current_microcode) {
		case 2:
			cpu.HLT = 1;
			break;
		default:
			break;
	}
	return;
}

//list of all the available opcodes

InstructionList instructions[16] = {
	{"NOP",NOP},{"LDA",LDA},{"ADD",ADD},{"SUB",SUB},{"STA",STA},{"LDI",LDI},{"JMP",JMP},{"JC",JC},
	{"JZ",JZ},{"---",NOP},{"---",NOP},{"---",NOP},{"---",NOP},{"---",NOP},{"OUT",OUT},{"HLT",HLT}
};

Cpu get_cpu()
{
	return cpu;
}

uint8_t* get_memory()
{
	uint8_t* memory = malloc(16);
	memcpy(memory,ram,16);
	return memory;
}

void load_from_bin(const char* filename)
{	
	FILE* bin_file = fopen(filename,"r");
	if(bin_file == NULL){
		fprintf(stderr,"Couldn't open binary file.");
		exit(-1);
	}

	fread(ram,1,16,bin_file);
	fclose(bin_file);
	return;
}

void load_from_asm(const char* filename)
{	
	char command[20 + strlen(filename)];
	strcat(command,"/bin/python3 asm.py ");
	strcat(command,filename);

	FILE* bin_file = popen(command,"r");
	if(bin_file == NULL){
		fprintf(stderr,"Couldn't open Assembly file.");
		exit(-1);
	}

	fread(ram,1,16,bin_file);
	return;
}

static uint8_t current_microcode;

uint8_t clock()
{	
	if(!cpu.HLT) {
		// each opcode is made up of 5 microcodes which takes 1 cycle each
		switch(current_microcode) {
			// First two microcode are same for all opcodes
			case 0:
				cpu.MAR = cpu.PC;
				break;
			case 1:
				cpu.IR = ram[cpu.MAR];
				cpu.PC++;
				if(cpu.PC > 15)
					cpu.PC = 0;
				break;
			default:
				uint8_t addr = cpu.IR >> 4;
				instructions[addr].operation(cpu.IR,current_microcode);
				break;
		}
	
		current_microcode = (current_microcode == 4)? 0 : (current_microcode+1);
		return 1;
	} else
		return 0;
}

uint8_t execute_ins()
{
	int first = 1;
	while(current_microcode || first) {
		first = 0;

		if(clock() == 0)
			return 0;
	}
	return 1;
}

char* get_curr_ins()
{	
	InstructionList curr_instruction = instructions[(cpu.IR & 0xf0) >> 4];
	char* dis_instruction = malloc(10);
	if(current_microcode < 2)
		sprintf(dis_instruction,"Fetching...");
	else
		sprintf(dis_instruction,"%s %d",curr_instruction.name,cpu.IR & 0x0f);
	return dis_instruction;
}
