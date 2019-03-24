/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 *
 *		Extra functionality:
 *		- checking for improper integer and floating-point literals
 */

# include <map>
# include <cstdio>
# include <cctype>
# include <cerrno>
# include <string>
# include <cstdlib>
# include <iostream>
# include "lexer.h"
# include "token.h"

using namespace std;
int numerrors, lineno = 1;


/* Later, we will associate token values with each keyword */

static map<int, string> keywords_map = {
	{AUTO, "auto"},
	{BREAK, "break"},
	{CASE, "case"},
	{CHAR, "char"},
	{CONST, "char"},
	{CONTINUE, "continue"},
	{DEFAULT, "default"},
	{DO, "do"},
	{DOUBLE, "double"},
	{ELSE, "else"},
	{ENUM, "enum"},
	{EXTERN, "extern"},
	{FLOAT, "float"},
	{FOR, "for"},
	{GOTO, "goto"},
	{IF, "if"},
	{INT, "int"},
	{LONG, "long"},
	{REGISTER, "register"},
	{RETURN, "return"},
	{SHORT, "short"},
	{SIGNED, "signed"},
	{SIZEOF, "sizeof"},
	{STATIC, "static"},
	{STRUCT, "struct"},
	{SWITCH, "switch"},
	{TYPEDEF, "typedef"},
	{UNION, "union"},
	{UNSIGNED, "unsigned"},
	{VOID, "void"},
	{VOLATILE, "volatile"},
	{WHILE, "while"},
};

static map<string, int> keywords = {
	{"auto", AUTO},
	{"break", BREAK},
	{"case", CASE},
	{"char", CHAR},
	{"const", CONST},
	{"continue", CONTINUE},
	{"default", DEFAULT},
	{"do", DO},
	{"double", DOUBLE},
	{"else", ELSE},
	{"enum", ENUM},
	{"extern", EXTERN},
	{"float", FLOAT},
	{"for", FOR},
	{"goto", GOTO},
	{"if", IF},
	{"int", INT},
	{"long", LONG},
	{"register", REGISTER},
	{"return", RETURN},
	{"short", SHORT},
	{"signed", SIGNED},
	{"sizeof", SIZEOF},
	{"static", STATIC},
	{"struct", STRUCT},
	{"switch", SWITCH},
	{"typedef", TYPEDEF},
	{"union", UNION},
	{"unsigned", UNSIGNED},
	{"void", VOID},
	{"volatile", VOLATILE},
	{"while", WHILE},
};

static map<int, string> operators = {
	{ASSIGN, "ASSIGN"},
	{LESSEQUAL, "LESSEQUAL"},
	{GREATEREQUAL, "GREATEREQUAL"},
	{ADD, "ADD"},
	{SUB, "SUB"},
	{STAR, "STAR"},
	{DIV, "DIV"},
	{MOD, "MOD"},
	{BINAND, "BINAND"}, 
	{NOT, "NOT"},
	{PERIOD, "PERIOD"},
	{LPARENTHESIS, "LPARENTHESIS"},
	{RPARENTHESIS, "RPARENTHESIS"},
	{LBRACKET, "LBRACKET"},
	{RBRACKET, "RBRACKET"},
	{LBRACE, "LBRACE"},
	{RBRACE, "RBRACE"},
	{SEMICOLON, "SEMICOLON"},
	{COLON, "COLON"},
	{COMMA, "COMMA"},
	{OR, "OR"},
	{AND, "AND"},
	{EQUAL, "EQUAL"},
	{NOTEQUAL, "NOTEQUAL"},
	{LESSTHAN, "LESSTHAN"},
	{GREATERTHAN, "GREATERTHAN"},
	{INCREMENT, "INCREMENT"},
	{DECREMENT, "DECREMENT"},
	{POINTER, "POINTER"},
};

/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

int lexan(string &lexbuf)
{
    long val;
    static int c = cin.get();


    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
	lexbuf.clear();


	/* Ignore white space */

	while (isspace(c)) {
	    if (c == '\n')
		lineno ++;

	    c = cin.get();
	}


	/* Check for an identifier or a keyword */

	if (isalpha(c) || c == '_') {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isalnum(c) || c == '_');

	    if (keywords.count(lexbuf) > 0)
		return keywords.at(lexbuf);

	    return ID;


	/* Check for a number (integer or real). */

	} else if (isdigit(c)) {
	    do {
		lexbuf += c;
		c = cin.get();
	    } while (isdigit(c));

	    if (c != '.') {
		errno = 0;
		val = strtol(lexbuf.c_str(), NULL, 0);

		if (errno != 0 || val != (int) val)
		    report("integer constant too large");

		return INTEGER;
	    }

	    lexbuf += c;
	    c = cin.get();

	    if (isdigit(c)) {
		do {
		    lexbuf += c;
		    c = cin.get();
		} while (isdigit(c));

		if (c == 'e' || c == 'E') {
		    lexbuf += c;
		    c = cin.get();

		    if (c == '-' || c == '+') {
			lexbuf += c;
			c = cin.get();
		    }

		    if (isdigit(c)) {
			do {
			    lexbuf += c;
			    c = cin.get();
			} while (isdigit(c));
		    } else
			report("missing exponent of floating-point constant");
		}
	    } else
		report("missing fractional part of floating-point constant");

	    errno = 0;
	    strtod(lexbuf.c_str(), NULL);

	    if (errno != 0)
		report("floating-point constant out of range");

	    return REAL;


	/* There must be an easier way to do this.  It might seem stupid at
	   this point to recognize each token separately, but eventually
	   we'll be returning separate token values to the parser, so we
	   might as well do it now. */

	} else {
	    lexbuf += c;

	    switch(c) {


	    /* Check for '||' */

	    case '|':
		c = cin.get();

		if (c == '|') {
		    lexbuf += c;
		    c = cin.get();
		    return OR;
		}

		break;


	    /* Check for '=' and '==' */

	    case '=':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return EQUAL;
		}

		return ASSIGN;


	    /* Check for '&' and '&&' */

	    case '&':
		c = cin.get();

		if (c == '&') {
		    lexbuf += c;
		    c = cin.get();
			return AND;
		}

		return BINAND;


	    /* Check for '!' and '!=' */

	    case '!':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return NOTEQUAL;
		}

		return NOT;


	    /* Check for '<' and '<=' */

	    case '<':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return LESSEQUAL;
		}

		return LESSTHAN;


	    /* Check for '>' and '>=' */

	    case '>':
		c = cin.get();

		if (c == '=') {
		    lexbuf += c;
		    c = cin.get();
			return GREATEREQUAL;
		}

		return GREATERTHAN;


	    /* Check for '-', '--', and '->' */

	    case '-':
		c = cin.get();

		if (c == '-') {
		    lexbuf += c;
		    c = cin.get();
			return DECREMENT;
		} else if (c == '>') {
		    lexbuf += c;
		    c = cin.get();
			return POINTER;
		}

		return SUB;


	    /* Check for '+' and '++' */

	    case '+':
		c = cin.get();

		if (c == '+') {
		    lexbuf += c;
		    c = cin.get();
			return INCREMENT;
		}

		return ADD;


	    /* Check for simple, single character tokens */

	    case '*':
			c = cin.get();
			return STAR;
		case '%':
			c = cin.get();
			return MOD;
		case ':':
			c = cin.get();
			return COLON;
		case ';':
			c = cin.get();
			return SEMICOLON;
	    case '(':
			c = cin.get();
			return LPARENTHESIS;
		case ')':
			c = cin.get();
			return RPARENTHESIS;
		case '[':
			c = cin.get();
			return LBRACKET;
		case ']':
			c = cin.get();
			return RBRACKET;
	    case '{':
			c = cin.get();
			return LBRACE;
		case '}':
			c = cin.get();
			return RBRACE;
		case '.':
			c = cin.get();
			return PERIOD;
		case ',':
			c = cin.get();
			return COMMA;


	    /* Check for '/' or a comment */

	    case '/':
		c = cin.get();

		if (c == '*') {
		    c = cin.get();

		    do {
			while (c != '*' && !cin.eof()) {
			    if (c == '\n')
				lineno ++;

			    c = cin.get();
			}

			c = cin.get();
		    } while (c != '/' && !cin.eof());

		    c = cin.get();
		    break;

		} else
		    return DIV;


	    /* Check for a string literal */

	    case '"':
		c = cin.get();

		while (c != '"' && c != '\n' && !cin.eof()) {
		    lexbuf += c;
		    c = cin.get();
		}

		if (c == '\n' || cin.eof())
		    report("premature end of string literal");

		lexbuf += c;
		c = cin.get();
		return STRING;


	    /* Handle EOF here as well */

	    case EOF:
		return DONE;


	    /* Ignore everything else */

	    default:
		c = cin.get();
		break;
	    }
	}
    }

    return DONE;
}


/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 */
/*
int main()
{
    int token;
    string lexbuf, type;

    while ((token = lexan(lexbuf)) != DONE) {
	switch(token) {
	case ID:
	    type = "identifier";
	    break;

	case INTEGER:
	    type = "integer";
	    break;

	case REAL:
	    type = "real";
	    break;

	case STRING:
	    type = "string";
	    break;
	}

	if(token >= AUTO and token <= WHILE) {
		type = keywords_map.at(token);
	}
	else if(token >= ASSIGN and token <= POINTER) {
		type = operators.at(token);
	}

	cout << type << ":" << lexbuf << endl;
    }

    return 0;
}
*/
