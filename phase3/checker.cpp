# include <iostream>
# include <string>
# include "checker.h"
# include "lexer.h"
# include "Type.h"
# include "Symbol.h"
# include "Scope.h"
# include "tokens.h"
# include <vector>

using namespace std;

std::vector<std::string> _defined;
Scope *currentScope, *globalScope = nullptr;//enclosing;

static string E1 = "redefinition of '%s'";
static string E2 = "redeclaration of '%s'";
static string E3 = "conflicting types for '%s'";
static string E4 = "'%s' undeclared";

void openScope() {
	currentScope = new Scope(currentScope);
	if(globalScope == nullptr)
		globalScope = currentScope;
	cout << "Open Scope" << endl;
}

void closeScope() {
	currentScope = currentScope->enclosing();
	cout << "Close Scope" << endl;
}

void defineFunction(const string &name, const Type &type) {
	for(unsigned i = 0; i<_defined.size(); i++) {
		if(name == _defined[i]) {
			report(E1, name);
			_defined.erase(_defined.begin() + i);
			break;
		}
	}
	_defined.push_back(name);
	declareFunction(name, type);
}

void declareFunction(const string &name, const Type &type) { //can be called from inner scope without error
	Symbol *symbol = globalScope->find(name);
	
	if(symbol == nullptr) {
		symbol = new Symbol(name, type);
		globalScope->insert(symbol);
	}
	else if(type != symbol->type()) {
		report(E3, name);
		globalScope->remove(name);
		globalScope->insert(symbol);
	}
}

void declareVariable(const string &name, const Type &type) {
	Symbol *symbol = currentScope->find(name);
	
	if(symbol == nullptr) {
		symbol = new Symbol(name, type);
		currentScope->insert(symbol);
	}
	else if(type != symbol->type()) {		
		report(E3, name);
		currentScope->remove(name);
		currentScope->insert(symbol);
	}
	else if(currentScope != globalScope ){
		report(E2, name);
		currentScope->remove(name);
		currentScope->insert(symbol);
	}
}

void checkIdentifier(const string &name) {
	if(currentScope->lookup(name) == nullptr) {
		report(E4, name);
		Symbol *symbol = new Symbol(name, Type());
		globalScope->insert(symbol);
	}
}

void checkFunction(const string &name) {
	if(currentScope->lookup(name) == nullptr) {
		Parameters *param = new Parameters();
		Symbol *symbol = new Symbol(name, Type(INT, 0, param));
		globalScope->insert(symbol);
	}
}
