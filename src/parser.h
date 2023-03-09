#ifndef URCL_PARSER
#define URCL_PARSER

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "utilities.h"

#define MAX_SNPRINTF 250

typedef unsigned int u_bitflag_t;

enum FatalParserErrors {
  NoError,
  UnexpectedEOF,
  ExpectedNumber,
  UnexpectedToken
};

struct operand_type_struct {
  bool no_arg : 1;
  bool immediate : 1;
  bool relative_mem : 1;
  bool program_counter : 1;
  bool is_register : 1;
  bool port : 1;
} __attribute__((packed));

enum OperandTypeEnum
{
  NOARG = 1,
  IMMEDIATE = 2,
  RELMEM = 6, // is an immediate by specification
  PROGRAMCOUNTER = 24, // is a register by specification
  REGISTER = 16,
  PORT = 32,
};

union OperandTypes {
  u_bitflag_t raw_value;
  enum OperandTypeEnum enum_value;
  struct operand_type_struct type;
};

#define TOP_OPERAND_TYPE PORT

static inline const char *get_type_name(u_bitflag_t type)
{
  switch (type)
  {
  case NOARG:
    return "no argument";
  case IMMEDIATE:
    return "immediate";
  case RELMEM:
    return "relative memory";
  case PROGRAMCOUNTER:
    return "PC";
  case REGISTER:
    return "register";
  case PORT:
    return "port";
  default:
    printf("unknown: %i\n", type);
    return "unknown";
  };
}

struct operand
{
  bool is_valid;
  union OperandTypes type;
  union
  {
    int64_t number;
    char port[32];
  } value;
};

static const char *InstructionNames[] = {"add", "rsh", "lod", "str",
                                         "bge", "nor", "imm"};

enum Instructions
{
  // core instructions
  ADD,
  RSH,
  LOD,
  STR,
  BGE,
  NOR,
  IMM,
  // basic instructions
};

static const u_bitflag_t register_or_imm = REGISTER | IMMEDIATE;

static u_bitflag_t instruction_arguments[3][7] = {
    {REGISTER, register_or_imm, register_or_imm},        // ADD
    {REGISTER, register_or_imm, NOARG},                  // RSH
    {REGISTER | PROGRAMCOUNTER, register_or_imm, NOARG}, // LOD
    {register_or_imm, register_or_imm, NOARG},           // STR
    {register_or_imm, register_or_imm, register_or_imm}, // BGE
    {REGISTER, register_or_imm, register_or_imm},        // NOR
    {REGISTER, IMMEDIATE, NOARG}                         // BGE
};

struct operand_error
{
  bool too_many_operands;
  bool not_enough_operands;
  bool unrecognized_instruction;
  
  char* error;
};

struct parsed_instruction
{
  enum Instructions instruction;

  struct operand destination;
  struct operand source1;
  struct operand source2;

  struct operand_error* error;
};

struct urcl_parser_state
{

  char *source;
  int pos;
  int col;
  int line;

  int instruction_count;
  int error_count;
  char* fatal_parser_error_src;
  enum FatalParserErrors fatal_parser_error_id;

  struct parsed_instruction *instructions;
  struct operand_error *errors;
};

struct urcl_parser_state *new_urcl_parser(char *input);
void state_push_new_error(struct urcl_parser_state* state, struct operand_error error);
bool urcl_parse_instruction(struct urcl_parser_state* state);
bool urcl_parser_advance_next_char(struct urcl_parser_state* state);
void urcl_parser_cleanup(struct urcl_parser_state *parser_state);


char *get_operand_type_descriptor(u_bitflag_t operand_type);
char *get_operand_error(u_bitflag_t instruction_bitflag, struct operand operand, char* operand_raw);
void operand_error_cleanup(struct operand_error error);

#endif
