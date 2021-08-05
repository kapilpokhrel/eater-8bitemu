#include <stdio.h>
#include <stdlib.h>
#include "computer.h"
#include <unistd.h>

int main()
{
	FILE* bin_file = fopen("programmes/jump.bin","r");
	if(bin_file == NULL){
		fprintf(stderr,"Couldn't open binary file.");
		exit(-1);
	}

	load(bin_file);

	int quit = 0;
	while(!quit) {
		execute_ins();

		// For now halt end the program.
		CPU_t cpu = get_cpu();
		printf("OUT: %d\n",cpu.OUT);
		sleep(1);

		if(cpu.HLT)
			quit = 1;
	}
}
