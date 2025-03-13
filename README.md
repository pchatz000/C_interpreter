# Simple Language interpreter in C.  

## Overview  
This project implements an interpreter for the **IPLI** language. I did this project in 2021 as an assignment in the course "Introduction to Programming".
The interpreter consists of three main components:  

1. **Tokenization** – Handled in `tokenizer.c`.  
2. **Parse Tree Construction** – Handled in `parser.c`.  
3. **Evaluation** – Also handled in `parser.c`.  

## Files  
The project consists of the following source files:  
- **Core implementation**: `bcc.c`, `tokenizer.c`, `parser.c`, `varfun.c`  
- **Header files**: `tokenizer.h`, `parser.h`, `varfun.h`  

## Compilation & Execution  
To compile the project, run:  
```sh
make ipli
```  
To run tests, execute:  
```sh
make tests
```  
The interpreter can be executed using:  
```sh
./ipli [-t] <iplfilename> [<arg1>] [<arg2>] ... [<argn>]
```  
- The `-t` flag enables a custom random number generator.  
- The `-v` flag cannot be used simultaneously with `-t`.  

## Interpreter Architecture  

### 1. Tokenization (`tokenizer.c`)  
- The tokenizer processes an `.ipl` file and stores each non-empty line in a **struct Line** array.  
- Each line contains a **struct Token** array representing different tokens (operators, variables, commands, etc.).  

### 2. Parse Tree Construction (`parser.c`)  
- The interpreter constructs a **tree-based structure** to execute commands.  
- Each line corresponds to a **node** in the tree.  
- Nodes have two pointers:  
  - `rightbro` → Points to the next node at the same indentation level.  
  - `child` → Points to the first command inside `if`, `else`, and `while` loops.  

### 3. Evaluation (`parser.c`)  
- The first token of each node determines the command type.  
- Each command has a dedicated function to handle its execution.  
- If an error occurs, `print_error()` prints an error message and terminates the program.  

## Variables (`varfun.c`)  
- Variables are stored in an **array** for efficient lookup (`O(1)`).  
- New variable insertion takes `O(n)`.  
- Arrays are stored as **tokens of type `kVariable`**.  
- Errors are raised if:  
  - A user redefines an existing array.  
  - A freed array is accessed without reinitialization.  

## Implemented Features  
✔ **Break & Continue with arguments** (`break <n>, continue <n>`)  
✔ **1D Arrays & Nested Expressions** (`a[b[i]]`)  
✔ **Logical Operators (`&&`, `||`)**  
✔ **Complex Expressions with Parentheses**  

## Expression Handling  
- Expressions are processed using the **Shunting Yard Algorithm** ([Wikipedia](https://en.wikipedia.org/wiki/Shunting-yard_algorithm)).  
- Operator precedence follows the C standard:  
  [Operator Precedence Table](https://en.cppreference.com/w/c/language/operator_precedence).  
- Supported operators:  
  `*, /, %, +, -, >, >=, <, <=, ==, !=, &&, ||`  

## Example Tests  
- **Break/Continue** → `tests/doubledigits/doubledigits.ipl`  
- **Counting Sort** → `tests/contingsort/countingsort.ipl`  
- **Complex Expressions** → `tests/expres/expres.ipl`  

## Check out the tests to see some examples
