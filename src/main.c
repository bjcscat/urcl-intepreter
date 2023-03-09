#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "parser.h"
#include "utilities.h"

int main(int argc, char** argv) {

	if (argc < 2) {
		puts("Missing file name.");
		return 1;
	}

	FILE* input_file = fopen(argv[1], "r");

	if (input_file == NULL) {
		puts("Failed to open file.");
		return 1;
	}

	fseek(input_file, 0L, SEEK_END);
	size_t size = ftell(input_file);
	fseek(input_file, 0L, SEEK_SET);

	char* input_buffer = malloc(size + 1);
	char* base_buffer = input_buffer;

	char tmp_chr = getc(input_file);
	while (tmp_chr != EOF) {
					*(input_buffer++) = tmp_chr;
					tmp_chr = getc(input_file);
	}
	*input_buffer = '\0';

	input_buffer = base_buffer;

	fclose(input_file);

	// start parsing

	struct urcl_parser_state *state = new_urcl_parser(input_buffer);
	
	bool parse_success = urcl_parse_instruction(state);

	if (!parse_success) {
		printf("%s\n", state->fatal_parser_error_src);
	}

	return 0;
}
