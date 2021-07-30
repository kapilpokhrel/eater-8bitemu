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
}CPU_t;

//road the binary file into ram
void load(FILE* bin_file);

//cpu information for debug information
CPU_t get_cpu();

uint8_t execute();

#endif