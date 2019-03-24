/*
Derek Nakamura
COEN 175
lexer.cpp - runs lexical analysis on input text and returns the matching lexicons
*/

# include <string>
# include <iostream>
# include <set>

using namespace std;

enum {
    KEYWORD, ID, INTEGER, REAL, STRING, OPERATOR, DONE,
};

//Set of possible keywords
std::set<std::string> keywords {"auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else", "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short", "signed", "sizeof", "static", "struct", "switch", "typedef","union", "unsigned", "void", "volatile", "while"};

//Set of possible operators
std::set<char> Operators {'*', '/', '%', '.', '(', ')', '[', ']', '{', '}', ';', ':', ',', '=', '+', '&', '-', '!', '<', '>'};

int lexan(string &lexbuf)
{
	char input = cin.peek();

	while(!cin.eof()){
		lexbuf.clear();

		//Whitespace (ignored)
		if(isspace(input)) {
			do {
				cin.get();
				input = cin.peek();
			}	while(isspace(input));
		}
		
		//Integer or Real Numbers
		if(isdigit(input)) {
			do {
				lexbuf += input;
				cin.get();
				input = cin.peek();
			}	while(isdigit(input));
			
			//Real Numbers
			if(input == '.') {
				cin.get();
				input = cin.peek();
				if(isdigit(input)) {
					lexbuf += '.';
					do {
						lexbuf += input;
						cin.get();
						input = cin.peek();
					}	while(isdigit(input));
					
					if(input == 'e' || input == 'E') {
						char temp1 = input;
						cin.get();
						input = cin.peek();
						if(input == '+' || input == '-') {
						char temp2 = input;
							cin.get();
							input = cin.peek();
							string tempbuf;
							if(isdigit(input)) {
								tempbuf += temp1;
								tempbuf += temp2;
								do {
									tempbuf += input;
									cin.get();
									input = cin.peek();
								}	while(isdigit(input));
	
								lexbuf += tempbuf;
								return REAL;
							}
						}
					}
					return REAL;
				}
				cin.putback('.');
			}

			//Integer
			return INTEGER;
		}
		
		//Keywords or Identifier
		if(isalpha(input) || input == '_') {
			do {
				lexbuf += input;
				cin.get();
				input = cin.peek();
			}	while(isalnum(input) || input == '_');
			
			//Keywords
			if(keywords.find(lexbuf) != keywords.end())
				return KEYWORD;

			return ID;
		}
		
		//Operators and Comments
		if(Operators.find(input) != Operators.end()) {
			lexbuf += input;
			char temp = input;
			cin.get();
			input = cin.peek();
			
			//Operators with two characters
			if(temp == '=' && input == '=') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;	
			}
			if(temp == '+' && input == '+') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			if(temp == '&' && input == '&') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			if((temp == '-' && input == '-') || (temp == '-' && input == '>')) {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			if(temp == '!' && input == '=') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			if(temp == '<' && input == '=') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			if(temp == '>' && input == '=') {
				lexbuf += input;
				cin.get();
				input = cin.peek();
				return OPERATOR;
			}
			
			//Comments
			if(temp == '/' && input == '*') {
				lexbuf.clear();
				while(1) {
					cin.get();
					input = cin.peek();
					if(input == '*'){
						temp = input;
						cin.get();
						input = cin.peek();
						if(input == '/') {
							cin.get();
							input = cin.peek();
							break;
						}
						else
							cin.putback(temp);
					}
				}
			}
			
			//All other Operators
			else
				return OPERATOR;
		}

		//Special Operator Case: || ('|' alone is not a valid operator)
		if(input == '|') {
			cin.get();
			input = cin.peek();
			if(input == '|'){
				cin.get();
				input = cin.peek();
				lexbuf += "||";
				return OPERATOR;
			}
		}

		//String
		if(input == '"') {
			do {
				lexbuf += input;
				cin.get();
				input = cin.peek();
			}	while(input != '"');
			lexbuf += input;
			cin.get();
			input = cin.peek();
			return STRING;
		}

		//All other Characters are Illegal Characters (ignored by lexer)
	}
    return DONE;
}

int main()
{
    int token;
    string lexbuf, type;

    while ((token = lexan(lexbuf)) != DONE) {
	switch(token) {
	case ID:
	    type = "identifier";
	    break;

	case KEYWORD:
	    type = "keyword";
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

	case OPERATOR:
	    type = "operator";
	    break;
	}

	cout << type << ":" << lexbuf << endl;
    }

    return 0;
}
