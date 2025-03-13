#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tokenizer.h"
#include "parser.h"
#include "varfun.h"


int main(int argc, char *argv[]) {
	FILE *fp;
	if (strcmp(argv[1], "-t") == 0) {
		fp = fopen(argv[2], "r");
		custom_random = 1;
	}	
	else if (strcmp(argv[1], "-v") == 0) {
		fp = fopen(argv[2], "r");
		print_commands = 1;
	}
	else fp = fopen(argv[1], "r");
	
	line_arr_size = 1000;
	line_arr = malloc(1000 * sizeof(struct Line));

	//1st part: tokenization
	read_file(fp); 
	#ifdef DEBUG
	debug_token();
	#endif
	
	//2nd part: parsing
	struct Node *root = build_tree(); 
	
	#ifdef DEBUG
	debug_tree(root);
	#endif

	//3rd part: evaluation of the parse tree
	eval(root, argc, argv, malloc(sizeof(long long)), malloc(sizeof(long long))); 


	return 0;
}