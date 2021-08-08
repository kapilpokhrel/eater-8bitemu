#include <stdio.h>
#include <stdlib.h>
#include "computer.h"
#include <unistd.h>

int main(int argc,char** argv)
{	
	if(argc < 2) {
		printf("Usage: <program_name> <-b or -a> <input_file>\n");
		exit(-1);
	}
	
	int loaded = 0;
	int opt;
	while((opt = getopt(argc,argv,"b:a:h")) != -1) {
		switch(opt) {
			case 'b':
				loaded = 1;
				load_from_bin(optarg);
				break;
			case 'a':
				loaded = 1;
				load_from_asm(optarg);
				break;
			case 'h':
				printf("Usage: <program_name> <-b or -a> <input_file>\n");
				exit(-1);
				break;
		}
	}

	if(!loaded) {
		printf("Couldn't load program.\n");
		exit(-1);
	}

	int quit = 0;
	while(!quit) {

		CPU_t cpu = get_cpu();
		printf("\t\tOUTPUT: %d\n\n",cpu.OUT);
		
		// CPU INFORMATION
		printf("=========CPU=========\n");
		printf("Program Counter: 0x%x\n",cpu.PC);
		printf("Memory address register: 0x%x\n",cpu.MAR);
		printf("Instruction register: 0x%x\n",cpu.IR);
		printf("A register: 0x%x\n",cpu.A);
		printf("B register: 0x%x\n",cpu.B);
		printf("SUM register: 0x%x\n",cpu.SUM);
		printf("OUT register: %x\n",cpu.OUT);
		printf("Carry flag: 0x%x\n",cpu.CY);
		printf("Zero flag: 0x%x\n",cpu.ZR);
		printf("=========CPU=========\n");

		// For now halt end the program.
		if(cpu.HLT)
			quit = 1;
		
		getc(stdin);
		clock();
		system("clear");
	}
}
