#ifndef COMPUTER_H
#define COMPUTER_H
#include <stdint.h>
#include <stdio.h>

typedef struct {
	//registers
	uint8_t PC; // Program counter
	uint8_t MAR; // Memory Address register
	uint8_t IR; // Instruction register
	uint8_t A; // A register
	uint8_t B; // B register
	uint8_t SUM; // Sum register (A+B)
	uint8_t OUT; //  Out register .. connected to display
	uint8_t CY; // Carry flag
	uint8_t ZR; // Zero flag
	uint8_t HLT;
}Cpu;

//road the binary file into ram
void load_from_bin(const char* filename);
void load_from_asm(const char* filename);

//cpu information for debug information
char* get_curr_ins();
Cpu get_cpu();
uint8_t* get_memory();
void set_memory(uint8_t address, uint8_t value);

uint8_t clock();

uint8_t execute_ins();

#endif
