#ifndef PARSER_H
#define PARSER_H

#include "tokenizer.h"

static char *err = "Error.";
static char *err_argument = "Invalid argument command.";
static char *err_assignment = "Expected assignment operator.";
static char *err_bad_indent = "Bad indentation.";
static char *err_div_zero = "Invalid division by zero.";
static char *err_else = "Else without if statement.";
static char *err_fewer_arguments = "Expected fewer arguments.";
static char *err_int = "Expected integer";
static char *err_invalid_array_declaration = "Invalid array declaration.";
static char *err_invalid_array_free = "Invalid array free.";
static char *err_invalid_breaks = "Invalid number of breaks.";
static char *err_invalid_continues = "Invalid number of continues.";
static char *err_invalid_expression = "Invalid expression.";
static char *err_invalid_token = "Invalid token.";
static char *err_mismatched_par = "Mismatched parenthesis.";
static char *err_more_arguments = "Expected more arguments.";
static char *err_var = "Expected variable.";
static char *err_varint = "Expected variable or constant.";
static char *err_invalid_command = "Invalid command given.";

struct Node {
	struct Line *ln;
	struct Node *child;
	struct Node *rightbro;
};

void eval(struct Node *, int, char *[], long long *, long long *);

struct Node *build_tree();
void print_error(char *, int);
void debug_tree(struct Node *);

extern int print_commands;
extern int custom_random;

#endif