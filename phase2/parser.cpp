# include <iostream>
# include <string>
# include <cstdlib>
# include "token.h"
# include "lexer.h"
# include "parser.h"

using namespace std;

static int lookahead;
static string lexbuf;

void error() {
	report("syntax error");
	exit(EXIT_FAILURE);
}

void match(int c) {
	if(lookahead != c) {
		error();
	}
	
	lookahead = lexan(lexbuf);
}

/* Specifier Functions */
bool is_specifier(int token) {
	if(token == CHAR || token == INT || token == DOUBLE) {
		return true;
	}
	
	return false;
}

void specifier() {
	if(is_specifier(lookahead)) {
		match(lookahead);
	} else {
		error();
	}
}

/* Pointers */
void pointers() {
	if(lookahead == STAR) {
		match(STAR);
		pointers();
	}
}

/* Expression Functions */
void primary_expression(bool lp) {
	if(lp) {
		expression();
		match(RPARENTHESIS);
	}
	else if(lookahead == ID) {
		match(ID);
		if(lookahead == LPARENTHESIS) {
			match(LPARENTHESIS);
			expression_list();
			match(RPARENTHESIS);
		}
	}
	else if(lookahead == REAL) {
		match(REAL);
	}
	else if(lookahead == INTEGER) {
		match(INTEGER);
	}
	else if(lookahead == STRING) {
		match(STRING);
	}
}

void array_expression(bool lp) {
	primary_expression(lp);
	
	if(lookahead == LBRACKET) {
		match(LBRACKET);
		expression();
		match(RBRACKET);
		array_expression(lp);
		cout << "index" << endl;
	}
}

void unary_expression() {
	if(lookahead == NOT) { 
		match(NOT);
		unary_expression();
		cout << "not" << endl;
	}
	else if(lookahead == SUB) {
		match(SUB);
		unary_expression();
		cout << "neg" << endl;
	}
	else if(lookahead == STAR) {
		match(STAR);
		unary_expression();
		cout << "deref" << endl;
	}
	else if(lookahead == BINAND) {
		match(BINAND);
		unary_expression();
		cout << "addr" << endl;
	}
	else if(lookahead == SIZEOF) {
		match(SIZEOF);
		cout << "sizeof" << endl;
		if(lookahead == LPARENTHESIS) {
			match(LPARENTHESIS);
			specifier();
			pointers();
			match(RPARENTHESIS);
			cout << "cast" << endl;
		}
	}
	else if(lookahead == LPARENTHESIS) {
		match(LPARENTHESIS);
		if(is_specifier(lookahead)) {
			specifier();
			pointers();
			match(RPARENTHESIS);
			unary_expression();
		}
		else {
			array_expression(true);
		}
	}
	
	else {
		array_expression(false);
	}
}

void multiplicative_expression() {
	unary_expression();
	
	if(lookahead == STAR) {
		match(STAR);
		unary_expression();
		multiplicative_expression();
		cout << "mul" << endl;
	}
	else if(lookahead == DIV) {
		match(DIV);
		unary_expression();
		multiplicative_expression();
		cout << "div" << endl;
	}
	else if(lookahead == MOD) {
		match(MOD);
		unary_expression();
		multiplicative_expression();
		cout << "rem" << endl;
	}
}

void additive_expression() {
	multiplicative_expression();
	
	if(lookahead == ADD) {
		match(ADD);
		multiplicative_expression();
		cout << "add" << endl;
	}
	else if (lookahead == SUB) {
		match(SUB);
		multiplicative_expression();
		cout << "sub" << endl;
	}
}

void comparison_expression() {
	additive_expression();
	
	if(lookahead == LESSTHAN) {
		match(LESSTHAN);
		additive_expression();
		cout << "ltn" << endl;
	}
	else if(lookahead == GREATERTHAN) {
		match(GREATERTHAN);
		additive_expression();
		cout << "gtn" << endl;
	}
	else if(lookahead == GREATEREQUAL) {
		match(GREATEREQUAL);
		additive_expression();
		cout << "geq" << endl;
	}
	else if(lookahead == LESSEQUAL) {
		match(LESSEQUAL);
		additive_expression();
		cout << "leq" << endl;
	}
}

void equality_expression() {
	comparison_expression();
	
	if(lookahead == EQUAL) {
		match(EQUAL);
		comparison_expression();
		cout << "eql" << endl;
	}
	else if(lookahead == NOTEQUAL) {
		match(NOTEQUAL);
		comparison_expression();
		cout << "neq"  << endl;
	}
}

void and_expression() {
	equality_expression();
	
	if(lookahead == AND) {
		match(AND);
		equality_expression();
		cout << "and" << endl;
	}
}

void expression() {
	and_expression();

	if(lookahead == OR) {
		match(OR);
		and_expression();
		cout << "or" << endl;
	}
}

void expression_list() {
	expression();
	
	if(lookahead == COMMA) {
		match(COMMA);
		expression_list();
	}
}

/* Statement Functions */
void statements() {
	while(lookahead != RBRACE) {
		statement();
	}
}

void statement() {
	if(lookahead == LBRACE) {
		match(LBRACE);
		declarations();
		statements();
		match(RBRACE);
	}
	
	else if(lookahead == RETURN) {
		match(RETURN);
		expression();
		match(SEMICOLON);
	}
	
	else if(lookahead == WHILE) {
		match(WHILE);
		match(LPARENTHESIS);
		expression();
		match(RPARENTHESIS);
		statement();
	}
	
	else if(lookahead == IF) {
		match(IF);
		match(LPARENTHESIS);
		expression();
		match(RPARENTHESIS);
		statement();
		if(lookahead == ELSE) {
			match(ELSE);
			statement();
		}
	}
	
	else {
		expression();
		if(lookahead == ASSIGN) {
			match(ASSIGN);
			expression();
		}
		match(SEMICOLON);
	}
}

/* Declaration Functions */
void declarator() {
	pointers();
	match(ID);
	if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(INTEGER);
		match(RBRACKET);
	}
}

void declarator_list() {
	declarator();
	
	if(lookahead == COMMA) {
		match(COMMA);
		declarator_list();
	}
}

void declaration() {
	specifier();
	declarator_list();
	match(SEMICOLON);
}

void declarations() {
	while(is_specifier(lookahead)) {
		declaration();
	}
}

/* Parameter Functions */
void parameter() {
	specifier();
	pointers();
	match(ID);
}

void parameter_list() {
	parameter();
	
	if(lookahead == COMMA) {
		match(COMMA);
		parameter_list();
	}
}

void parameters() {
	if(lookahead == VOID) {
		match(VOID);
	}
	
	else {
		parameter_list();
	}
}

/* Func or Global */
void global_declarator() {
	pointers();
	match(ID);
	
	if(lookahead == LPARENTHESIS) {
		match(LPARENTHESIS);
		parameters();
		match(RPARENTHESIS);
	}
	
	else if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(INTEGER);
		match(RBRACKET);
	}
}

void remaining_decls() {
	if(lookahead == SEMICOLON) {
		match(SEMICOLON);
	}
	
	else if(lookahead == COMMA) {
		match(COMMA);
		global_declarator();
		remaining_decls();
	}
}

void func_or_global() {
	specifier();
	pointers();
	match(ID);
	
	if(lookahead == LBRACKET) {
		match(LBRACKET);
		match(INTEGER);
		match(RBRACKET);
		remaining_decls();
	} else if(lookahead == LPARENTHESIS){
		match(LPARENTHESIS);
		parameters();
		match(RPARENTHESIS);
		
		if(lookahead == LBRACE) {
			match(LBRACE);
			declaration();
			statements();
			match(RBRACE);
		} else {
			remaining_decls();
		}
	} else {
		remaining_decls();
	}
}


/* Main */
int main() {
	lookahead = lexan(lexbuf);
	while (lookahead != DONE) {
		func_or_global();
	}
	
	exit(1);
}
