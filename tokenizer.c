#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tokenizer.h"
#include "varfun.h"
#include "parser.h"

#define MAX_LINE_LENGTH 1000

int line_arr_size = 0;
int absolute_line = 1;

int is_var_int(struct Token *tk) {
	return (tk->type == kVariable || tk->type == kConstant);
}

int is_number(char *word) {
	int n = strlen(word);
	for (int i=0; i<n; i++) {
		if (word[i]<'0' || word[i]>'9') return 0;
	}
	return 1;
}

int is_valid_variable(char *p) {
	int n = strlen(p);
	for (int i=0; i<n; i++) {
		if (p[i]>='a' && p[i]<='z') continue;
		if (p[i]>='A' && p[i]<='Z') continue;
		if (p[i]>='0' && p[i]<='9') continue;
		if (p[i]=='_') continue;
		if (p[i]=='[') continue;
		if (p[i]==']') continue;
		return 0;
	}
	if (p[0] == '_' || (p[0]>='0' && p[0]<='9') || p[0] == '[' || p[0] == ']') return 0;
	return 1;
}

void determine_type (struct Token *tk);

int array_name_split(struct Token *tk) {
	int i = 0;
	int n = strlen(tk->word);
	while (tk->word[i] != '[' && i<n) {
		i++;
	}
	if (tk->word[i] == '[') {
		tk->index = malloc(sizeof(struct Token));
		
		if (tk->word[n-1] != ']') {
			return 0;
		} 
		
		tk->word[i] = '\0';
		i++;
		
		for (int j=0; i<n-1; j++, i++) {
			tk->index->word[j] = tk->word[i];
		}
		tk->has_index = 1;
		determine_type(tk->index);
		if (!is_var_int(tk->index)) return 0;
	}

	return 1;

}

void determine_type (struct Token * tk) {
	if (strcmp(tk->word, "argument") == 0) {
		tk->type = kArgument;
	}
	else if (strcmp(tk->word, "break") == 0) {
		tk->type = kBreak;
	}
	else if (strcmp(tk->word, "continue") == 0) {
		tk->type = kContinue;
	}
	else if (strcmp(tk->word, "&&") == 0) {
		tk->type = kLogicAnd;
	}
	else if (strcmp(tk->word, "||") == 0) {
		tk->type = kLogicOr;
	}
	else if (strcmp(tk->word, "(") == 0) {
		tk->type = kOpenPar;
	}
	else if (strcmp(tk->word, ")") == 0) {
		tk->type = kClosePar;
	}
	else if (strcmp(tk->word, "random") == 0) {
		tk->type = kRandom;
	}
	else if (strcmp(tk->word, "srandom") == 0) {
		tk->type = kSrandom;
	}
	else if (strcmp(tk->word, "current_time") == 0) {
		tk->type = kCurrentTime;
	} 
	else if (strcmp(tk->word, "size") == 0) {
		tk->type = kSize;
	}
	else if (strcmp(tk->word, "new") == 0) {
		tk->type = kNew;
	}
	else if (strcmp(tk->word, "free") == 0) {
		tk->type = kFree;
	}
	else if (strcmp(tk->word, "if") == 0) {
		tk->type = kIf;
	}
	else if (strcmp(tk->word, "else") == 0) {	
		tk->type = kElse;
	}
	else if (strcmp(tk->word, "while") == 0) {
		tk->type = kWhile;
	}
	else if (strcmp(tk->word, "read") == 0) {
		tk->type = kRead;
	}
	else if (strcmp(tk->word, "write") == 0) {
		tk->type = kWrite;
	}
	else if (strcmp(tk->word, "writeln") == 0) {
		tk->type = kWriteLn;
	}
	else if (strcmp(tk->word, "==") == 0) {
		tk->type = kEqual;
	}
	else if (strcmp(tk->word, ">") == 0) {
		tk->type = kGreater;
	}
	else if (strcmp(tk->word, ">=") == 0) {
		tk->type = kGreaterEqual;
	}
	else if (strcmp(tk->word, "<") == 0) {
		tk->type = kLess;
	}
	else if (strcmp(tk->word, "<=") == 0) {
		tk->type = kLessEqual;
	}
	else if (strcmp(tk->word, "=") == 0) {
		tk->type = kAssignment;
	}
	else if (strcmp(tk->word, "+") == 0) {
		tk->type = kPlus;
	}
	else if (strcmp(tk->word, "-") == 0) {
		tk->type = kMinus;
	}
	else if (strcmp(tk->word, "*") == 0) {
		tk->type = kMul;
	}
	else if (strcmp(tk->word, "/") == 0) {
		tk->type = kDiv;
	}
	else if (strcmp(tk->word, "%") == 0) {
		tk->type = kMod;
	}
	else if (strcmp(tk->word, "!=") == 0) {
		tk->type = kNotEqual;
	} 
	else if (is_number(tk->word))  {
		tk->num = atoi(tk->word); 
		tk->type = kConstant;
	}
	else if (is_valid_variable(tk->word)) {
		tk->type = kVariable;
		if (!array_name_split(tk)) {
			tk->type = kUnknown;
		}
		else {
			tk->varpos = var_insert(tk->word);
		}
	}

}

void print_type(struct Token* tk) { // its used to debug
  switch(tk->type) {
  	case kCurrentTime:
  		printf("kCurrentTime");
  		break;
  	case kSrandom:
  		printf("kSrandom");
  		break;
  	case kLogicOr:
  		printf("kLogicOr");
  		break;
  	case kLogicAnd:
  		printf("kLogicAnd");
  		break;
  	case kOpenPar:
  		printf("kOpenPar");
  		break;
	case kClosePar:
  		printf("kClosePar");
  		break;
  	case kSize:
  		printf("kSize");
  		break;
  	case kRandom:
  		printf("kRandom");
  		break;
  	case kUnknown:
		printf("kUknown");
		break;
    case kArgument:
		printf("kArgument");
		break;
    case kConstant:
		printf("kConstant:%lld", tk->num);
		break;
    case kVariable:
		printf("kVariable: %s ", tk->word);
    	if (tk->has_index) {
    		print_type(tk->index);
    	}
		break;
	case kNew:
		printf("kNew");
		break;
	case kFree:
		printf("kFree");
		break;
	case kIf:
		printf("kIf");
		break;
	case kElse:
		printf("kElse");
		break;
	case kRead:
		printf("kRead");
		break;
	case kWrite:
		printf("kWrite");
		break;
	case kWriteLn:
		printf("kWriteLn");
		break;
	case kEqual:
		printf("kEqual");
		break;
	case kLess:
		printf("kLess");
		break;
	case kLessEqual:
		printf("kLessEqual");
		break;
	case kGreater:
		printf("kGreater");
		break;
	case kGreaterEqual:
		printf("kGreaterEqual");
		break;
	case kAssignment:
		printf("kAssignment");
		break;
	case kWhile:
		printf("kWhile");
		break;
	case kMod:
		printf("kMod");
		break;
	case kPlus:
		printf("kPlus");
		break;
	case kDiv:
		printf("kDiv");
		break;
	case kMinus:
		printf("kMinus");
		break;
	case kMul:
		printf("kMul");
		break;
	default:
		printf("unknown:%s", tk->word);
  }
}

int is_operator(char x) { 
	return (x == '=' || x == '-' || x == '+' || x == '/' || x == '*' || x == '%'
		|| x == '&' || x == '|' || x == '<' || x == '>');
}

//reads and creates an alphanumeric token
//this token can contain open and closed brackets and underscores
void read_alnum(struct Line *line, char *cur_ln, int *i) { 
	int j = 0;
	do {
		line->tokens[line->token_cnt].word[j] = cur_ln[*i];
		j++;
		(*i)++;
	} while (isalnum(cur_ln[*i]) || cur_ln[*i] == '_');
	if (cur_ln[*i] == '[') {
		int brackets = 0;
		do {
			if (cur_ln[*i] == '[') brackets++;
			if (cur_ln[*i] == ']') brackets--;

			line->tokens[line->token_cnt].word[j] = cur_ln[*i];
			j++;
			(*i)++;
		} while (brackets > 0 && cur_ln[*i] != '\0');

		if (cur_ln[*i] == ']') {
			line->tokens[line->token_cnt].word[j] = cur_ln[*i];
			j++;
			(*i)++;
		}
	}
	line->tokens[line->token_cnt].word[j] = '\0';
	line->token_cnt++;
}

//reads and creates an operator token
void read_operator(struct Line *line, char *cur_ln, int *i) { 
	int j = 0;
	do {
		line->tokens[line->token_cnt].word[j] = cur_ln[*i];
		j++;
		(*i)++;
	} while (is_operator(cur_ln[*i]));
	line->tokens[line->token_cnt].word[j] = '\0';
	line->token_cnt++;
}

//reads a line from the input and breaks it into tokens
int read_line(FILE * fp, struct Line *line) { 
	char cur_ln[MAX_LINE_LENGTH];
	if (fgets(cur_ln, MAX_LINE_LENGTH, fp) == NULL) return 0;
	int line_length = strlen(cur_ln);
	int i;
	line->indent = 0;
	for (i = 0; cur_ln[i] == '\t'; i++) line->indent++;

	while (i<line_length) {
		if (isalnum(cur_ln[i])) {
			read_alnum(line, cur_ln, &i);
		}
		else if (isspace(cur_ln[i])) {
			i++;
		}
		else if (cur_ln[i] == '#') {
			break;
		}
		else if (cur_ln[i] == ')' || cur_ln[i] == '(') {
			line->tokens[line->token_cnt].word[0] = cur_ln[i];
			line->tokens[line->token_cnt].word[1] = '\0';
			line->token_cnt++;
			i++;
		}
		else {
			read_operator(line, cur_ln, &i);
		}
	}

	for (int j=0; j<line->token_cnt; j++) {
		//printf("%s\n", line->tokens[j].word);
		determine_type(&line->tokens[j]);
	}
	return 1;
}

void read_file(FILE * fp) {
	int ret = 1;
	while(1) {
		if (line_cnt == line_arr_size-3) { 
			line_arr_size += 1000;
			line_arr = realloc(line_arr, (line_arr_size)*sizeof(struct Line));
		}
		ret = read_line(fp, &line_arr[line_cnt]);
		if (ret == 0) break;
		line_arr[line_cnt].abs_ln = absolute_line;
		if (line_arr[line_cnt].token_cnt > 0) line_cnt++;
		absolute_line++;
	}
	line_arr[line_cnt+1].indent = -1;

}

void debug_token() {
	for (int i=0; i<line_cnt; i++) {
		printf("line: %d, %d, indent: %d, token_cnt: %d, tokens:", i, line_arr[i].abs_ln, line_arr[i].indent, line_arr[i].token_cnt);
		for (int j=0; j<line_arr[i].token_cnt; j++) {
			printf("\n\t\t\t'%s', type:", line_arr[i].tokens[j].word);
			print_type(&line_arr[i].tokens[j]);
			printf("\n");
		}
		printf("\n");
	}
}