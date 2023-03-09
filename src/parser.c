#include <ctype.h>

#include "parser.h"

void state_push_new_error(struct urcl_parser_state* state, struct operand_error error) {
	state->errors = realloc(state->errors, (state->error_count + 1) * sizeof(struct operand_error));
	state->errors[state->error_count++] = error;
}

void _invoke_parser_fatal_error(struct urcl_parser_state *state, enum FatalParserErrors id, char *input)
{
	state->fatal_parser_error_id = id;
	state->fatal_parser_error_src = input;
}

char *get_allocated_str_behind_whitespace(char *input)
{
	size_t str_size = strlen(input);
	char *advance_input = input;
	for (; !isspace(*advance_input) && *advance_input; advance_input++)
	{
	}

	size_t length = (advance_input - input);

	// printf("%i, %i\n", sizeof(struct sized_str) + sizeof(char[length]), length);
	char *result = malloc(length);

	strncpy(result, input, length);
	result[length] = '\0';

	return result;
}

char *get_operand_type_descriptor(u_bitflag_t operand_type)
{
	int total_size = 0;
	int current_place_totals = 0;
	const char *totals[sizeof(struct operand_type_struct) * 8];
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
	memset(result_str, 0, total_size + (4 * current_place_totals - 1));

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
						struct operand operand, char *operand_raw)
{
	const char *format_str = NULL;
	char *result = NULL;
	if ((instruction_bitflag & operand.type.raw_value) == 0)
	{
		if (instruction_bitflag == NOARG)
		{
			return "Too many operands to instruction";
		}
		else
		{
			format_str = "Expected type `%s` got '%s' (%s)";
		}
	}

	if (format_str)
	{
		char *expected_types = get_operand_type_descriptor(instruction_bitflag);

		result = format_allocate_str(format_str, expected_types, operand_raw, get_type_name(operand.type.enum_value));

		free(expected_types);

		return result;
	}

	return NULL;
}

struct operand get_operand(struct urcl_parser_state *state, u_bitflag_t field_bitflag, char *operand_raw)
{

	struct operand result_operand = {false, 0, 0};
	int get_num_str_offset = 0;
	int get_num_val_offset = 0;

	switch (operand_raw[0])
	{
	case '~':
	{
		result_operand.type.type.immediate = true;
		get_num_str_offset = 1;
		get_num_val_offset = state->instruction_count;
		goto get_number;
		break;
	}
	case '#':
	case 'm':
	case 'M':
		result_operand.type.type.immediate = true;
		result_operand.type.type.relative_mem = true;
		get_num_str_offset = 1;
		goto get_number;
		break;
	case '$':
	case 'r':
	case 'R':
		result_operand.type.type.is_register = true;
		get_num_str_offset = 1;
		goto get_number;
		break;
	case 'p':
		if (*(operand_raw + 1) != 'c')
		{
			result_operand.type.type.no_arg = true;
			_invoke_parser_fatal_error(state, UnexpectedToken, format_allocate_str("Unexpected token '%c'", *(operand_raw + 1)));
			break;
		}
	case 'P':
		if (*(operand_raw + 1) != 'C')
		{
			result_operand.type.type.no_arg = true;
			_invoke_parser_fatal_error(state, UnexpectedToken, format_allocate_str("Unexpected token '%c'", *(operand_raw + 1)));
			break;
		}
		result_operand.type.type.is_register = true;
		result_operand.type.type.program_counter = true;
		break;
	default:
	{
	get_number:
	{

		bool is_cont_digit = true;
		for (char *scan = operand_raw + 1; *scan; scan++)
		{
			if (!isdigit(*scan))
			{
				is_cont_digit = false;
				break;
			}
		}
		if (is_cont_digit)
		{
			result_operand.value.number = atoi(operand_raw + get_num_str_offset) + get_num_val_offset;
		}
		else
		{
			_invoke_parser_fatal_error(state, ExpectedNumber, format_allocate_str("Unexpected token."));
			return result_operand;
		}
		printf("%i %i %s\n", result_operand.type.raw_value, result_operand.value.number, state->fatal_parser_error_src);
		break;
	}
	}
	};

	char* inst_error = get_operand_error(field_bitflag, result_operand, operand_raw);

	if (inst_error) {
		state_push_new_error(state, (struct operand_error) {false, true, false, inst_error});
		result_operand.is_valid = false;
	} else {
		result_operand.is_valid = true;
	}

	return result_operand;
}

struct urcl_parser_state *new_urcl_parser(char *input)
{
	struct urcl_parser_state *new_state = malloc(sizeof(struct urcl_parser_state));

	*new_state = (struct urcl_parser_state){
		input, // source
		0,	   // pos
		0,	   // line
		0,	   // col

		0,		 // instruction_ount
		0,		 // error_count
		NULL,	 // fatal_parser_error_text
		NoError, // fatal_parser_error_id

		malloc(sizeof(struct parsed_instruction) * 8), // instructions
		malloc(sizeof(struct operand_error))	   // errors
	};

	return new_state;
}

bool urcl_parser_advance_next_char(struct urcl_parser_state *parser_state)
{
	while (isspace(parser_state->source[parser_state->pos]))
	{
		parser_state->pos++;
	}
	return parser_state->source[parser_state->pos] == '\0';
}

void urcl_parser_cleanup(struct urcl_parser_state *parser_state)
{
	for (int i = parser_state->error_count - 1; i > 0; i--)
	{
		operand_error_cleanup(parser_state->errors[i]);
	}

	if (parser_state->fatal_parser_error_src)
	{
		free(parser_state->fatal_parser_error_src);
	}

	free(parser_state->errors);

	free(parser_state->instructions);
}

bool urcl_parse_instruction(struct urcl_parser_state *state)
{
	struct parsed_instruction new_instruction;

	char *instruction_str = get_allocated_str_behind_whitespace(state->source + state->pos);
	state->pos += strlen(instruction_str) + 1;

	bool is_null = urcl_parser_advance_next_char(state);

	if (is_null)
	{
		state->fatal_parser_error_src = format_allocate_str("Unexpected <eof>");
		state->fatal_parser_error_id = UnexpectedEOF;
		return false;
	}

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

	if (!found_search)
	{

		state_push_new_error(state, (struct operand_error) {true, false, false, format_allocate_str("Unknown instruction `%s`\n", instruction_str)});

		return false;
	}

	printf("inst: %s\n", instruction_str);

	free(instruction_str);

	int *argument_types = instruction_arguments[new_instruction.instruction];

	char *dest_operand_str = get_allocated_str_behind_whitespace(state->source + state->pos);
	state->pos += strlen(dest_operand_str) + 1;

	new_instruction.destination = get_operand(state, argument_types[0], dest_operand_str);

	printf("dest: %s\n", dest_operand_str);

	free(dest_operand_str);

	char *dest_src1_str = get_allocated_str_behind_whitespace(state->source + state->pos);

	state->pos += strlen(dest_src1_str) + 1;

	printf("src1: %s\n", dest_src1_str);

	free(dest_src1_str);

	char *dest_src2_str = get_allocated_str_behind_whitespace(state->source + state->pos);
	state->pos += strlen(dest_src2_str) + 1;

	printf("src2: %s\n", dest_src2_str);

	state->instructions[state->instruction_count++] = new_instruction;

	free(dest_src2_str);

	return true;
}

void operand_error_cleanup(struct operand_error error)
{
	free(error.error);
}
