//varfun.h handles all the processes concerning variables 

#ifndef VARFUN_H
#define VARFUN_H

#include "tokenizer.h"

typedef long long ll;

int var_cnt;

struct VarState { 
	char *varname; //variable name
	int is_array;
	int array_size;
	ll value; //variable value
	ll *array;
} vars[1000]; //the array in which i store the variables

int var_insert(char *);
ll get(struct Token *, int);
void set(struct Token *, ll, int);
ll evalVarInt(struct Token *, int); 
int new_array(struct Token *, int);
int free_array(struct Token *);
int get_array_size(struct Token *, struct Token *, int);

#endif
