#ifndef URCL_PARSER
#define URCL_PARSER

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SNPRINTF 250

typedef unsigned int u_bitflag_t;

enum OperandTypes
{
  NOARG = 1,
  IMMEDIATE = 2,
  PROGRAMCOUNTER = 4, // bruh
  REGISTER = 8,
  PORT = 16,
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
  case PROGRAMCOUNTER:
    return "PC";
  case REGISTER:
    return "register";
  case PORT:
    return "port";
  default:
    fputs("Failed to get type of operand", stderr);
    exit(1);
  }
}

struct operand
{
  enum OperandTypes type;
  union
  {
    int64_t number;
    u_int32_t register_id;
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

static u_bitflag_t instruction_arguments[][3] = {
    {REGISTER, register_or_imm, register_or_imm},        // ADD
    {REGISTER, register_or_imm, NOARG},                  // RSH
    {REGISTER | PROGRAMCOUNTER, register_or_imm, NOARG}, // LOD
    {register_or_imm, register_or_imm, NOARG},           // STR
    {register_or_imm, register_or_imm, register_or_imm}, // BGE
    {REGISTER, register_or_imm, register_or_imm},        // NOR
    {REGISTER, IMMEDIATE, NOARG}                         // BGE
};

struct instruction_error
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

  struct instruction_error* error;
};


struct urcl_parser_state
{

  char *source;
  int pos;
  int col;
  int line;

  int instruction_count;
  int error_count;

  struct parsed_instruction *instructions;
  struct instruction_error *errors;
};

struct urcl_parser_state *new_urcl_parser(char *input);
void urcl_parser_cleanup(struct urcl_parser_state *parser_state);

bool parse_instruction(struct urcl_parser_state* state);

char *get_operand_type_descriptor(u_bitflag_t operand_type);
char *get_operand_error(u_bitflag_t instruction_bitflag, struct operand operand);
void instruction_error_cleanup(struct instruction_error error);

#endif