#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <string.h>

enum TokenType{kUnknown, kArgument, kConstant, kVariable, kIf, kElse, 
	kRead, kWrite, kWriteLn, kEqual, kGreater, kLess, kLessEqual,
	kGreaterEqual, kAssignment, kTab, kEOL, kWhile, kMod, kPlus, 
	kDiv, kMinus, kMul, kNotEqual, kRandom, kSrandom, kSize, kOpenPar, kClosePar,
	kLogicAnd, kLogicOr, kCurrentTime, kBreak, kContinue, kNew, kFree};

typedef long long ll;

struct Token {
	enum TokenType type; 
	char word[100]; //the string of the token
	ll num; //if the token is a constant, i store its value here
	int has_index; //is 1 if the variable token has index
	int varpos; //if the token is a variable, i store its position on the variable array so that i can access it in o(1)
	struct Token *index;
};

struct Line { //the array of all non-empty the lines
	struct Token tokens[500]; //the array of tokens in every line
	int abs_ln; //the absolute line number, taking into account empty lines 
	int token_cnt; // the number of tokens in the line
	int indent;	//the indentation of the line
};

int line_cnt; //the number of non empty lines in a file
extern int line_arr_size; //the size of the line array
struct Line *line_arr;

void read_file(FILE *);
void debug_token();
int is_var_int(struct Token *);
void print_type(struct Token *);

#endif