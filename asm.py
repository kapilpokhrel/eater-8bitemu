#!/bin/python3
from math import *
from sys import argv, stdout
import getopt

def error(message):
	print(message)
	exit(-1)

opcodes = {
	"nop": 0,
	"lda": 1,
	"add": 2,
	"sub": 3,
	"sta": 4,
	"ldi": 5,
	"jmp": 6,
	"jc": 7,
	"jz": 8,
	"out": 14,
	"hlt": 15
}

# 1st pass
# Remove comments, and add lables in symbol talbe and store int instruction in tokens form
def first_pass(lines):
	lables = dict()
	line_tokens = list()

	curr_addr = 0
	for line in lines:
		# Remove comments
		modified_line = line.split(";")[0]
		# Remove whitespaces from beginning and end
		modified_line = modified_line.strip()
		# Split label and instruction
		modified_line = modified_line.split(":")

		# if line has label add it in symbol table and remove it from line
		if(len(modified_line) > 1):
			lables[modified_line[0]] = curr_addr
			modified_line = modified_line[1]
		else:
			modified_line = modified_line[0]

		if(modified_line != ""):
			curr_addr += 1

		tokens = modified_line.split(maxsplit=1)
		line_tokens.append(tokens)
	
	return lables,line_tokens

# 2nd pass
# Replace the lables, does error checking, and convert the assembly instructions in byte form
def second_pass(lines,lables,line_tokens):
	curr_line = 1
	bytes = []

	# Go through each line and parse the tokens
	for tokens in line_tokens:
		if(tokens != []):
			# storing the original from of tokens for showing errors
			orig_tokens = list(tokens)

			# Parse and change into numeric form

			# replace the opcode if present with its real numeric value
			if tokens[0] in opcodes:
				tokens[0] = str(opcodes[tokens[0]] << 4)
			try:
				tokens[0] = eval(tokens[0])
			except:
				error(f"Error on line {curr_line - 1}.\n\t{lines[curr_line - 1].strip()}\n"
					+ f"Couldn't parse '{orig_tokens[0]}'")

			if(len(tokens) > 1):
				# if operand contains any label, replace it with label value
				for label in lables:
					tokens[1] = tokens[1].replace(label,str(lables[label]))
				try:
					tokens[1] = eval(tokens[1])
				except:
					error(f"Error on line {curr_line - 1}.\n\t{lines[curr_line - 1].strip()}\n"
						+ f"Couldn't parse '{orig_tokens[1]}'")

				# operand must be of 4 bits; not greater than 15
				if(tokens[1] > 15):
						error(f"Error on line {curr_line - 1}.\n\t{lines[curr_line - 1].strip()}\n"
							+ f"Operand Exceeds 4 bits (greater than 15)")

			# Store the parsed data

			if(len(tokens) > 1):
				bytes.append(tokens[0] | tokens[1])
			else:

				# can't exceed a byte
				if(tokens[0] > 255):
					error(f"Error on line {curr_line - 1}.\n\t{lines[curr_line - 1].strip()}\n"
						+ f"Instruction Exceeds a byte")
				else:
					bytes.append(tokens[0])

		curr_line += 1

	return bytearray(bytes)

def main(argv):
	
	if(len(argv) < 2):
		error("Usage: ./asm.py <input_file> [-o output_file]")
	input_file = argv[1]
	output_file = ""

	options = "o:"
	arguments,values = getopt.getopt(argv[2:],options)
	for argument,value in arguments:
		if(argument == "-o"):
			output_file = value
		else:
			error(f"Unknown argument {argument}")
	
	lines = list()
	try:
		with open(input_file) as asm_file:
			lines = asm_file.readlines()
	except:
		error(f"Couldn't open file")
	
	lables,line_tokens = first_pass(lines)
	bytes = second_pass(lines,lables,line_tokens)

	# Output bytes
	if(output_file != ""):
		with open(output_file,"wb") as out_file:
			out_file.write(bytes)
	
	stdout.buffer.write(bytes)

# Start
if __name__ == "__main__":
	main(argv)