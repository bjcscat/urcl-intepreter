#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "parser.h"

int main(void) {
	//new_instruction("andf ");
	/*
	FILE* input_file = fopen("input.urcl", "r");
	struct stat input_stat;

	int result = stat("input.urcl", &input_stat);

	char* input_buffer = malloc(input_stat.st_size);
	char* base_buffer = input_buffer;

	char tmp_chr = getc(input_file);
	while (tmp_chr != EOF) {
					*(input_buffer++) = tmp_chr;
					tmp_chr = getc(input_file);
	}
	*(input_buffer++) = '\0';

	input_buffer = base_buffer;

	fclose(input_file);

	// start parsing

	unsigned int instruction_index = 0;
	char* line = strtok(input_buffer, "\n");
	while (line != NULL) {
					char Instruction[7];
					int scan_result = sscanf(line, "%6s", Instruction);
					if (scan_result == EOF) {
									printf("Error reading line");
									break;
					}
					puts(Instruction);

					instruction_index++;
					line = strtok(NULL, "\n");
	}

	free(base_buffer);
	*/
	return 0;
}