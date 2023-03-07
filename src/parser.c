#include <ctype.h>

#include "parser.h"

char *get_allocated_str_behind_whitespace(char *input)
{
	char *advance_input = input;
	for (; !isspace(*advance_input); advance_input++)
	{
	}

	size_t length = advance_input - input;
	char *result = malloc(length + 1);

	strncpy(result, input, length);
	result[length] = '\0';

	return result;
}

char *get_operand_type_descriptor(u_bitflag_t operand_type)
{
	int total_size = 0;
	int current_place_totals = 0;
	const char *totals[sizeof(enum OperandTypes) * 8];
	for (u_bitflag_t operand_check = TOP_OPERAND_TYPE; operand_check > NOARG; operand_check >>= 1)
	{
		if ((operand_type & operand_check) != 0)
		{
			const char *result_name = get_type_name(operand_check);
			total_size += strlen(result_name);
			totals[current_place_totals++] = result_name;
		}
	}

	char *result_str = (char *)malloc(total_size + (4 * current_place_totals - 1)); // end part is for the ` or `

	int index = 0;
	for (; index < current_place_totals - 1; index++)
	{
		strcat(result_str, totals[index]);
		strcat(result_str, " or ");
	}
	strcat(result_str, totals[index]);

	return result_str;
}

char *get_operand_error(u_bitflag_t instruction_bitflag,
						struct operand operand)
{
	const char *format_str = NULL;
	char *result = NULL;
	if ((instruction_bitflag & operand.type) == 0)
	{
		if (instruction_bitflag == NOARG)
		{
			return "Too many operands to instruction";
		}
		else
		{
			format_str = "Expected `%s` got `%s`";
		}
	}

	if (format_str)
	{
		char *expected_types = get_operand_type_descriptor(instruction_bitflag);
		int result_size = snprintf(NULL, 0, format_str, expected_types, get_type_name(operand.type));

		result = (char *)malloc(result_size);

		sprintf(result, format_str, expected_types, get_type_name(operand.type));

		free(expected_types);

		return result;
	}

	return NULL;
}

struct urcl_parser_state *new_urcl_parser(char *input)
{
	struct urcl_parser_state *new_state = malloc(sizeof(struct urcl_parser_state));

	*new_state = (struct urcl_parser_state){
		input, // source
		0,	   // pos
		0,	   // line
		0,	   // col

		0, // instruction_ount
		0, // error_count

		malloc(sizeof(struct parsed_instruction) * 8), // instructions
		malloc(sizeof(struct instruction_error))};		   // errors

	return new_state;
}

void urcl_parser_cleanup(struct urcl_parser_state *parser_state) {
	for (int i = parser_state->instruction_count - 1;i > 0;i--) {
		instruction_error_cleanup(parser_state->errors[i]);
	}

	free(parser_state->errors);

	free(parser_state->instructions);
}

bool parse_instruction(struct urcl_parser_state *state)
{
	struct parsed_instruction new_instruction;

	char *instruction_str = get_allocated_str_behind_whitespace(state->source);
	bool found_search = false;

	for (int i = 0; i < sizeof(InstructionNames) / sizeof(InstructionNames[0]); i++)
	{
		if (strcmp(InstructionNames[i], instruction_str) == 0)
		{
			new_instruction.instruction = (enum Instructions)i;
			found_search = true;
			break;
		}
	}

	if (found_search)
	{
		free(instruction_str);	

		state->instructions[state->instruction_count++] = new_instruction;

		return true;
	} else {
		struct instruction_error err = {
			false,
			false,
			true,

			NULL
		};
		int size = snprintf(NULL, 0, "Unknown instruction `%s`\n", instruction_str);

		err.error = malloc(size);

		sprintf(err.error, "Unknown instruction `%s`\n", instruction_str);

		state->errors[state->error_count++] = err;

		return false;
	}

}

void instruction_error_cleanup(struct instruction_error error) {
	free(error.error);
}