#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "parser.h"
#include "tokenizer.h"
#include "varfun.h"

typedef long long ll;

int custom_random; //print_commands is 1 if "-t" flag was given and 0 otherwise
int print_commands; //print_commands is 1 if "-v" flag was given and 0 otherwise
int while_loops = 0; //the number of while loops we are inside

ll random_arr[60];
ll seed = 123456789; 

//this function creates pseudo-random integers
ll get_random_integer() {
	//this function always returns 4
	//chosen by fair dice roll
	//guaranteed to be random
	
	
	seed = (1103515245 * seed + 12345) % 2147483648;
	
	return seed;
}

void debug_tree(struct Node *id) {

	for (int i=0; i < id->ln->indent; i++) {
		printf("\t");
	}
	printf("%d\n", id->ln->abs_ln);
	if (id->child != NULL) debug_tree(id->child);
	if (id->rightbro != NULL) debug_tree(id->rightbro);

}

//Returns the precedence of an operator. Higher precedence means higher operation priority.
int precedence(struct Token *op) {
	if (op->type == kMul || op->type == kDiv || op->type == kMod) return 5; //*, /, %

	else if (op->type == kPlus || op->type == kMinus) return 4; //+, -

	else if (op->type == kGreater || op->type == kGreaterEqual || op->type == kLess 
			|| op->type == kLessEqual) return 3; //>, >=, <, <=

	else if (op->type == kEqual || op->type == kNotEqual) return 2; //==, !=

	else if (op->type == kLogicAnd || op->type == kLogicOr) return 1; //&&, ||

	else return 0;
}

//returns 1 if the token is an operator, 0 otherwise
int is_op_token(struct Token *tk) { 
	return (precedence(tk)>0);
}

//prints the error message and finishes the program
void print_error(char *error_message, int linenumb) { 
	fprintf(stderr, "%d: %s\n", linenumb, error_message);
	exit(1);
}

//prints the command currently run, only if the interpreter is run with the "-v" flag
void print_line(struct Line *ln) { 
	fprintf(stderr, "%4d:  ", ln->abs_ln);
	for (int i=0; i < ln->indent; i++) fprintf(stderr, "\t");
	for (int i=0; i < ln->token_cnt; i++) fprintf(stderr, " %s", ln->tokens[i].word);
	fprintf(stderr, "\n");
}

//builds the parse tree
struct Node *build_tree() { 
	static int i; 
	struct Node *root = malloc(sizeof(struct Node));
	struct Node *id = root;
	while (1) {
		id->ln = &line_arr[i];
		if (line_arr[i+1].indent > line_arr[i].indent) {
			if (line_arr[i+1].indent != line_arr[i].indent+1) {
				print_error(err_bad_indent, line_arr[i+1].abs_ln);
			}
			if (id->ln->tokens[0].type != kIf && id->ln->tokens[0].type != kElse && id->ln->tokens[0].type != kWhile) {
				print_error(err_bad_indent, line_arr[i+1].abs_ln);
			}
			i++;
			id->child = build_tree();
		}
		if (line_arr[i+1].indent == id->ln->indent) {
			i++;
			id->rightbro = malloc(sizeof(struct Node));
			id = id->rightbro;
		}
		else break;
	}
	return root;
}

//evaluates the argument command
void evalArgument(struct Node *id, int argc, char *argv[]) {
	struct Line *cur = id->ln;

	if (cur->token_cnt == 3 && 
		is_var_int(&cur->tokens[1]) && 
		cur->tokens[2].type == kVariable) {
		ll index = evalVarInt(&(cur->tokens[1]), cur->abs_ln) + print_commands + custom_random + 1;
		if (index >= argc) print_error(err_argument, cur->abs_ln);
		set(&cur->tokens[2], atoi(argv[index]), cur->abs_ln);
	} 
	else if (cur->token_cnt == 3 && cur->tokens[1].type == kSize && cur->tokens[2].type == kVariable) {
		set(&cur->tokens[2], argc - print_commands - custom_random - 2, cur->abs_ln);
	}
	else { 
		print_error(err_argument, cur->abs_ln);
	}
}

//evaluates the "write" and "writeln" commands
void evalWrite(struct Node *id) {
	struct Line *cur = id->ln;
	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	else if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln);
	else if (!is_var_int(&cur->tokens[1])) print_error(err_varint, cur->abs_ln);
	
	printf("%lld", evalVarInt(&cur->tokens[1], cur->abs_ln));

	if (cur->tokens[0].type == kWrite) printf(" ");
	else printf("\n"); 
}

//evaluates the "read" command 
void evalRead(struct Node *id) {
	struct Line *cur = id->ln;
	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln);
	if (cur->tokens[1].type != kVariable) print_error(err_var, cur->abs_ln);
	long long  tmp;
	scanf("%lld", &tmp);
	set(&cur->tokens[1], tmp, cur->abs_ln);
}

//returns the result of a binary operation between the numbers a and b
ll evalBinOperation (ll a, ll b, struct Token *tk, int abs_line) { 
	ll val;
	switch (tk->type) {
		case kPlus:
			val = a + b;
			break;
		case kMinus:
			val = a - b;
			break;
		case kMul:
			val = a * b;
			break;
		case kDiv:
			if (b == 0) print_error(err_div_zero, abs_line);
			val = a / b;
			break;
		case kMod:
			if (b == 0) print_error(err_div_zero, abs_line);
			val = a % b;
			break;
		case kGreater:
			val = a > b;
			break;
		case kGreaterEqual:
			val = a >= b;
			break;
		case kLess:
			val = a < b;
			break;
		case kLessEqual:
			val = a <= b;
			break;
		case kEqual:
			val = a == b;
			break;
		case kNotEqual:
			val = a != b;
			break;
		case kLogicOr:
			val = a || b;
			break;
		case kLogicAnd:
			val = a && b;
			break;
	}
	return val;
}

// evaluates a complex expression using the shanting yard algorithm and returns the result
ll evalExpression(int start_token, struct Line *cur) { 
	//in the rpn array, the output of the shanting yard algorithm is stored, in reverse polish notation
	struct Token *rpn[1000], *opstack[1000]; 
	int rpnsize = 0, opsize = 0;

	//start of the algorithm
	for (int i=start_token; i<cur->token_cnt; i++) { 

		struct Token *tk = &cur->tokens[i];

		if (is_var_int(tk)) {
			rpn[rpnsize++] = tk;
		}
		else if (is_op_token(tk)) {
			while (opsize > 0 
				&& precedence(opstack[opsize-1]) >= precedence(tk)
				&& opstack[opsize-1]->type != kOpenPar) {

				rpn[rpnsize++] = opstack[--opsize];

			}
			opstack[opsize++] = tk;
		}
		else if (tk->type == kOpenPar) {
			opstack[opsize++] = tk;
		} 
		else if (tk->type == kClosePar) {
			while (opsize > 0 && opstack[opsize-1]->type != kOpenPar) {
				rpn[rpnsize++] = opstack[--opsize];
			}
			if (opsize == 0) print_error(err_mismatched_par, cur->abs_ln);
			
			if (opstack[opsize-1]->type == kOpenPar) --opsize;
		}
		else print_error(err_invalid_expression, cur->abs_ln);
	}
	while (opsize > 0) {
		if (opstack[opsize-1]->type == kOpenPar) print_error(err_mismatched_par, cur->abs_ln);
		rpn[rpnsize++] = opstack[--opsize];
	}
	//end of the algorithm

	//calculating the expression in reverse polish notation
	ll ans[1000]; 
	int anssize = 0;
	for (int i=0; i<rpnsize; i++) {

		if (is_op_token(rpn[i])) {
			if (anssize < 2) print_error(err_invalid_expression, cur->abs_ln);
			ll tmp = evalBinOperation(ans[anssize-2], ans[anssize-1], rpn[i], cur->abs_ln);
			anssize -= 2;
			ans[anssize++] = tmp;
		}
		
		else {
			ans[anssize++] = evalVarInt(rpn[i], cur->abs_ln);
		}
	}
	if (anssize != 1) print_error(err_invalid_expression, cur->abs_ln);

	return ans[0];
}

//evaluates a condition
//the condition is implemented as an expression
//so you can use all the operators both on expressions and conditions just like in c
ll evalCondition(int start_token, struct Line *cur) { 
	return evalExpression(start_token, cur)!=0;
}

//evaluates the assignment to a variable
void evalAssignment(struct Node *id) { 
	struct Line *cur = id->ln;
	if (cur->token_cnt < 3) print_error(err_more_arguments, cur->abs_ln);
	if (cur->tokens[1].type != kAssignment) print_error(err_assignment, cur->abs_ln);
	set(&cur->tokens[0], evalExpression(2, cur), cur->abs_ln);
}


//evaluates the if command
int evalIf(struct Node *id, int argc, char *argv[], ll *continue_levels, ll *break_levels) { // returns 1 if the condition was true and 0 otherwise.
	struct Line *cur = id->ln;
	if (evalCondition(1, cur)) {
		eval(id->child, argc, argv, continue_levels, break_levels);
		return 1;
	}
	return 0;
}

//evaluates the else command
void evalElse(int if_condition, struct Node *id, int argc, char *argv[], ll *continue_levels, ll *break_levels) {
	// if there was not an if statement before the else statement if_condition = -1
	if (if_condition == -1) { 
		print_error(err_else, id->ln->abs_ln);
	}
	else if (if_condition == 0) eval(id->child, argc, argv, continue_levels, break_levels); //if there was an if statement and its condition was false, that means we want to evaluate else
}

//evaluates the while command
void evalWhile(struct Node *id, int argc, char *argv[], ll *continue_levels, ll *break_levels) {
	struct Line *cur = id->ln;
	while (evalCondition(1, cur)) {
		if (print_commands == 1) print_line(cur);
		while_loops++;
		//calls the eval function to run the while block
		eval(id->child, argc, argv, continue_levels, break_levels);
		while_loops--;
		if (*break_levels > 0) {
			(*break_levels)--;
			break;
		}
		if (*continue_levels > 0) {
			(*continue_levels)--;
		}
	}
}

void evalRandom(struct Node *id) {
	struct Line *cur = id->ln;
	
	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	else if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln); 
	
	if (cur->tokens[1].type != kVariable) print_error(err_var, cur->abs_ln);
	if (!custom_random) set(&cur->tokens[1], rand(), cur->abs_ln);
	else set(&cur->tokens[1], get_random_integer(), cur->abs_ln);
}

//this function evaluates the command srandom. it is used as srand() in c.
//"srandom current_time" is the equivalent of srand(time(NULL)) in my implementation
void evalSRandom(struct Node *id) { 
	struct Line *cur = id->ln;
	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln); 

	if ( !is_var_int(&cur->tokens[1]) && cur->tokens[1].type != kCurrentTime) print_error(err_varint, cur->abs_ln);
	if ( cur->tokens[1].type == kCurrentTime) srand(time(NULL));
	else srand(evalVarInt(&cur->tokens[1], cur->abs_ln));
}

//break function actually sets the value of break_levels
ll evalBreak(struct Node *id) {
	struct Line *cur = id->ln;
	ll breakl = 0;

	if (cur->token_cnt == 1) breakl = 1;
	else if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	else {
		if (cur->tokens[1].type != kConstant) print_error(err_int, cur->abs_ln);
		if (cur->tokens[1].num <= 0) print_error(err, cur->abs_ln);
		breakl = cur->tokens[1].num;
	}
	if (while_loops < breakl) print_error(err_invalid_breaks, cur->abs_ln);
	return breakl;
}

//continue fucntion implements the continue command
//continue <n> is equivalent to break <n-1> and continue 1
//so i set continue_levels and break_levels variables to these values
void evalContinue(struct Node *id, ll *continue_levels, ll *break_levels) {
	struct Line *cur = id->ln;
	ll continuel = 0;

	if (cur->token_cnt == 1) continuel = 1;
	else if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	else {
		if (cur->tokens[1].type != kConstant) print_error(err_int, cur->abs_ln);
		if (cur->tokens[1].num <= 0) print_error(err, cur->abs_ln);
		continuel = cur->tokens[1].num;
	}

	if (while_loops < continuel) print_error(err_invalid_continues, cur->abs_ln);

	*break_levels = continuel - 1;
	*continue_levels = 1;
}


//creates a new array
void evalNew(struct Node *id) {
	struct Line *cur = id->ln;

	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln);
	if (cur->tokens[1].index == NULL) print_error(err_invalid_array_declaration, cur->abs_ln);
	if (!new_array(&cur->tokens[1], cur->abs_ln)) print_error(err_invalid_array_declaration, cur->abs_ln);

}

//frees an array
void evalFree(struct Node *id) {
	struct Line *cur = id->ln;

	if (cur->token_cnt > 2) print_error(err_fewer_arguments, cur->abs_ln);
	if (cur->token_cnt < 2) print_error(err_more_arguments, cur->abs_ln);
	if (!free_array(&cur->tokens[1])) print_error(err_invalid_array_free, cur->abs_ln);
}

//gets the size of an array
void evalSize(struct Node *id) {
	struct Line *cur = id->ln;

	if (cur->token_cnt > 3) print_error(err_fewer_arguments, cur->abs_ln);
	if (cur->token_cnt < 3) print_error(err_more_arguments, cur->abs_ln);
	if (cur->tokens[2].type != kVariable) print_error(err_var, cur->abs_ln);

	if (!get_array_size(&cur->tokens[1], &cur->tokens[2], cur->abs_ln)) print_error(err, cur->abs_ln);
}

void eval(struct Node *id, int argc, char *argv[], ll *continue_levels, ll *break_levels) {
	int if_condition;
	while (id != NULL) {
		struct Line *cur = id->ln;
		if (print_commands == 1 && cur->abs_ln != 0 && cur->tokens[0].type != kWhile) 
			print_line(cur);
		
		switch(cur->tokens[0].type) {
			case kArgument:
				evalArgument(id, argc, argv);
				break;
			case kWrite:
				evalWrite(id);
				break;
			case kWriteLn:
				evalWrite(id);
				break;
			case kRead:
				evalRead(id);
				break;
			case kVariable:
				evalAssignment(id);
				break;
			case kIf:
				if_condition = evalIf(id, argc, argv, continue_levels, break_levels);
				break;
			case kElse:
				evalElse(if_condition, id, argc, argv, continue_levels, break_levels);
				break;
			case kWhile:
				evalWhile(id, argc, argv, continue_levels, break_levels);
				break;
			case kRandom:
				evalRandom(id);
				break;
			case kSrandom:
				evalSRandom(id);
				break;
			case kBreak:
				*break_levels = evalBreak(id);
				break;
			case kContinue:
				evalContinue(id, continue_levels, break_levels);
				break;
			case kNew:
				evalNew(id);
				break;
			case kFree:
				evalFree(id);
				break;
			case kSize:
				evalSize(id);
				break;
			default:
				//if (cur->abs_ln != 0) print_error(err_invalid_command, id->ln->abs_ln);
				break;
		}

		if (cur->tokens[0].type != kIf) if_condition = -1;

		if (*break_levels > 0 || *continue_levels > 0) break;
		
		id = id->rightbro;
	}
}
