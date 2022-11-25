#include <stdio.h>
#include <stdlib.h>
#include "computer.h"
#include <unistd.h>
#include <ncurses.h>
#include <panel.h>

void print_output(WINDOW* output_window,uint8_t value)
{
	mvwprintw(output_window,0,1,"OUTPUT:");
	mvwprintw(output_window,1,1," %03d ",value);
}

void print_cpu(WINDOW* cpu_window,Cpu cpu) 
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
}

void print_memory(WINDOW* memory_window, uint8_t selected_address)
{	
	uint8_t* memory = get_memory();
	mvwprintw(memory_window,0,1,"MEMORY:");
	for(int i = 0; i < 16; i++) {
		if(i == selected_address)
			wattron(memory_window, A_STANDOUT);
		mvwprintw(memory_window,i+1,1," %02d: %03d ",i,memory[i]);
		wattroff(memory_window, A_STANDOUT);
	}
	free(memory);
}

void print_instruction(WINDOW* ins_window)
{
	char* instruction = get_curr_ins();
	mvwprintw(ins_window,0,1,"Current Instruction:");
	mvwprintw(ins_window,1,1,"                    "); /*Clearing the previous instruction*/
	mvwprintw(ins_window,1,1," %s",instruction);
	free(instruction);
}

void print_usage(WINDOW* usage_window)
{
	mvwprintw(usage_window,0,1,"USAGE:");
	mvwprintw(usage_window,1,1," [c][CLOCK] [s][STEP INSTRUCTION] [q][Exit] [e][Edit]");
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
	cbreak();
	noecho();

	// Get screen size
	int y,x;
	getmaxyx(def_win,y,x);

	WINDOW* main_window = newwin(24,60,y/2-12,x/2-30);
	keypad(main_window, true);
	box(main_window,0,0);
	mvwprintw(main_window,0,1,"Eater's 8 bit computer:");
	
	// Output window
	WINDOW* out_win = derwin(main_window,3,10,2,2);
	box(out_win,0,0);
	// Cpu window
	WINDOW* cpu_win = derwin(main_window,12,35,5,2);
	box(cpu_win,0,0);
	// Memory window
	WINDOW* mem_win = derwin(main_window,18,11,2,47);
	box(mem_win,0,0);
	// Instruction window
	WINDOW* ins_win = derwin(main_window,3,35,17,2);
	box(ins_win,0,0);
	// Usage window
	WINDOW* usage_win = derwin(main_window,3,56,20,2);
	box(usage_win,0,0);
	print_usage(usage_win);

	PANEL* main_panel = new_panel(main_window);

	uint8_t quit = 0;
	uint8_t selected_address = 0;
	uint8_t edit = 0;
	while(!quit) {
		//manage_windows(def_win,out_win,cpu_win,mem_win,usage_win);
		Cpu cpu = get_cpu();

		// OUTPUT
		print_output(out_win,cpu.OUT);
		
		// CPU INFORMATION
		print_cpu(cpu_win,cpu);
	
		// MEMORY INFORMATION
		print_memory(mem_win, edit?selected_address:-1);

		// Usage Information
		print_instruction(ins_win);

		// Update panel and show it
		update_panels();
		doupdate();

		refresh();
		int ch = wgetch(main_window);
		uint8_t* memory = get_memory();
		uint8_t selected_value = memory[selected_address];

		if(ch == 'c')
			clock();
		else if(ch == 's')
			execute_ins();
		else if(ch == 'q')
			quit = 1;
		else if(ch == 'e')
			edit ^= 1;
		else if(ch == KEY_UP && edit) {
			if(selected_address != 0)
				selected_address -= 1;
		}
		else if(ch == KEY_DOWN && edit) {
			if(selected_address != 15)
				selected_address += 1;
		}
		else if(ch == KEY_RIGHT && edit) {
			if(selected_value != 255)
				set_memory(selected_address, selected_value + 1);
		}
		else if(ch == KEY_LEFT && edit) {
			if(selected_value != 0)
				set_memory(selected_address, selected_value - 1);
		}

		
		// Always center the window
		getmaxyx(def_win,y,x);
		move_panel(main_panel,y/2-12,x/2-30);
	}
	
	// ncurses end
	endwin();
}
