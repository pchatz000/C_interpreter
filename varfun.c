#include <limits.h>
#include <stdlib.h>

#include "varfun.h"
#include "tokenizer.h"
#include "parser.h"

typedef long long ll;

// Inserts a new value if it does not exist. 
//In any case, it returns the index of the variable in the variable array
int var_insert(char *p) { 

	for (int i=0; i<var_cnt; i++) {

		if (strcmp(vars[i].varname, p) == 0) {
			//if (vars[i].is_array != is_array) print_error(err, line);
			return i;  
		}
	}
	vars[var_cnt].is_array = -1;
	vars[var_cnt].varname = p;
	vars[var_cnt].value = 0;
	var_cnt++;
	return (var_cnt-1);
}

//returns the value of a variable
ll get(struct Token *tk, int abs_ln) { 
	if (tk->has_index == 1) {
		if (vars[tk->varpos].is_array != 1) print_error(err, abs_ln);
		ll index = evalVarInt(tk->index, abs_ln);
		if (index < 0 || index >= vars[tk->varpos].array_size) print_error(err, abs_ln);
		return vars[tk->varpos].array[index];
	}
	else {
		if (vars[tk->varpos].is_array == 1) print_error(err, abs_ln);
		vars[tk->varpos].is_array = 0;
		return vars[tk->varpos].value;
	}
}

//changes the value of a variable 
void set(struct Token *tk, ll newval, int abs_ln) { 
	if (tk->has_index == 1) {
		if (vars[tk->varpos].is_array != 1) print_error(err, abs_ln);
		ll index = evalVarInt(tk->index, abs_ln);
		if (index < 0 || index >= vars[tk->varpos].array_size) print_error(err, abs_ln);
		vars[tk->varpos].array[index] = newval;
	}
	else {
		if (vars[tk->varpos].is_array == 1) print_error(err, abs_ln);
		vars[tk->varpos].is_array = 0;
		vars[tk->varpos].value = newval;
	}
}

//if the token is a Constant or a Variable it returns its value
ll evalVarInt(struct Token *x, int abs_ln) {
	if (x->type == kConstant) return x->num;
	else if (x->type == kVariable) return get(x, abs_ln);
}

//creates a new array
int new_array(struct Token *tk, int abs_ln) {
	if (vars[tk->varpos].is_array != -1) { 
		return 0;
	}

	vars[tk->varpos].is_array = 1;

	if (tk->has_index == 0 || !is_var_int(tk->index)) return 0;
	
	ll index_value = evalVarInt(tk->index, abs_ln);
	
	if (index_value <= 0) return 0;
		
	vars[tk->varpos].array = malloc(index_value*sizeof(ll));
	
	//if malloc failed to allocate memory:
	if (vars[tk->varpos].array == NULL) return 0; 
	vars[tk->varpos].array_size = index_value;
	
	for (int i=0; i<vars[tk->varpos].array_size; i++) {
		vars[tk->varpos].array[i] = 0;
	}	

	return 1;
}

int free_array(struct Token *tk) {
	if (tk->has_index) return 0;
	if (vars[tk->varpos].is_array != 1) return 0;
	vars[tk->varpos].is_array = -1;
	free(vars[tk->varpos].array);
}

int get_array_size(struct Token *ar, struct Token *var, int abs_ln) {
	if (ar->has_index) return 0;
	if (vars[ar->varpos].is_array != 1) return 0;
	set(var, vars[ar->varpos].array_size, abs_ln);
}

