#include <stdio.h>
#include <stdlib.h>
#include "computer.h"
#include <unistd.h>
#include <ncurses.h>

void print_output(WINDOW* output_window,uint8_t value)
{
	mvwprintw(output_window,0,1,"OUTPUT:");
	mvwprintw(output_window,1,1," %03d ",value);
	
	wrefresh(output_window);
}

void print_cpu(WINDOW* cpu_window,CPU_t cpu) 
{
	mvwprintw(cpu_window,0,1,"CPU INFORMATION:");
	mvwprintw(cpu_window,1,1," Program Counter: 0x%x",cpu.PC);
	mvwprintw(cpu_window,2,1," Memory address register: 0x%x",cpu.MAR);
	mvwprintw(cpu_window,3,1," Instruction register: 0x%x",cpu.IR);
	mvwprintw(cpu_window,4,1," A register: 0x%x",cpu.A);
	mvwprintw(cpu_window,5,1," B register: 0x%x",cpu.B);
	mvwprintw(cpu_window,6,1," SUM register: 0x%x",cpu.SUM);
	mvwprintw(cpu_window,7,1," OUT register: 0x%x",cpu.OUT);
	mvwprintw(cpu_window,8,1," Carry flag: %d",cpu.CY);
	mvwprintw(cpu_window,9,1," Zero flag: %d",cpu.ZR);
	mvwprintw(cpu_window,10,1," HLT flag: %d",cpu.HLT);

	wrefresh(cpu_window);
}

void print_memory(WINDOW* memory_window)
{	
	uint8_t* memory = get_memory();
	mvwprintw(memory_window,0,1,"MEMORY:");
	for(int i = 1; i <= 16; i++) {
		mvwprintw(memory_window,i,1," %02d: %03d",i-1,memory[i-1]);
	}
	free(memory);

	wrefresh(memory_window);
}

void print_usage(WINDOW* usage_window)
{
	mvwprintw(usage_window,0,1,"USAGE:");
	mvwprintw(usage_window,1,1," [c][CLOCK] [s][STEP INSTRUCTION]");
	
	wrefresh(usage_window);
}

void manage_windows(WINDOW* main, WINDOW* out_win,WINDOW* cpu_win,WINDOW* mem_win,WINDOW* usage_win)
{	
	refresh();

	int main_x, main_y;
	getmaxyx(main,main_y,main_x);
	int start_x, start_y;

	// To justify the windows at center
	start_x = main_x/2 - 25; // width of whole content is 50
	start_y = main_y/2 - 9; // height of whole content is 18

	box(out_win,0,0);
	box(cpu_win,0,0);
	box(mem_win,0,0);
	box(usage_win,0,0);

	/*
		start_y + value or start_x + value
		value depends of the width or height of window previous to it

		for example:
			height of output_window = 2
			so, starting value of cpu_window which is below output_window = 3
	*/
	mvwin(out_win,start_y+0,start_x+1);
	mvwin(cpu_win,start_y+3,start_x+1);
	mvwin(mem_win,start_y+0,start_x+38);
	mvwin(usage_win,start_y+15,start_x+1);

	wrefresh(cpu_win);
	wrefresh(out_win);
	wrefresh(mem_win);
	wrefresh(usage_win);

	refresh();
}

int main(int argc,char** argv)
{	
	if(argc < 3) {
		printf("Usage: <program_name> < -b or -a > <input_file>\n");
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
				printf("Usage: <program_name> < -b or -a > <input_file>\n");
				exit(-1);
				break;
		}
	}

	if(!loaded) {
		printf("Couldn't load program.\n");
		exit(-1);
	}
	
	// ncurses init
	WINDOW* def_win = initscr();
	noecho();

	/*
		WINDOW* newwin(height,widht,y,x); newwin decleration
		width and height is managed by manage_windows function.
	*/

	// Output window
	WINDOW* out_win = newwin(3,10,0,0);	
	// Cpu window
	WINDOW* cpu_win = newwin(12,35,0,0);	
	// Memory window
	WINDOW* mem_win = newwin(18,12,0,0);
	// Usage window
	WINDOW* usage_win = newwin(3,35,0,0);

	manage_windows(def_win,out_win,cpu_win,mem_win,usage_win);
	print_usage(usage_win);
	
	int quit = 0;

	while(!quit) {
		manage_windows(def_win,out_win,cpu_win,mem_win,usage_win);

		CPU_t cpu = get_cpu();
	
		// OUTPUT
		print_output(out_win,cpu.OUT);
		
		// CPU INFORMATION
		print_cpu(cpu_win,cpu);
	
		// MEMORY INFORMATION
		print_memory(mem_win);

		char ch = getch();
		if(ch == 'c')
			clock();
		else if(ch == 's')
			execute_ins();
		else if(ch == 'q')
			quit = 1;
		
		clear();
	}
	
	// ncurses end
	endwin();
}
