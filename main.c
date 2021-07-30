#include <stdio.h>
#include <stdlib.h>
#include "computer.h"

int main()
{
	FILE* bin_file = fopen("programmes/add","r");
	if(bin_file == NULL){
		fprintf(stderr,"Couldn't open binary file.");
		exit(-1);
	}

	load(bin_file);

	int quit = 0;
	while(!quit) {
		execute();

		// For now halt end the program.
		CPU_t cpu = get_cpu();
		if(cpu.HLT)
			quit = 1;
	}
}