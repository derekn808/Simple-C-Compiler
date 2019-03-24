/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <set>
# include <iostream>
# include <string>
# include "lexer.h"
# include "checker.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static set<string> funcdefns;
static Scope *outermost, *toplevel;
static const Type error;
static const Type integer(INT);
static const Type dbl(DOUBLE);

static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string undeclared = "'%s' undeclared";

static string invalid_return = "invalid return type";
static string invalid_type = "invalid type for test expression";
static string lvalue_required = "lvalue required in expression";
static string invalid_operands = "invalid operands to binary %s";
static string invalid_operand_unary = "invalid operand to unary %s";
static string invalid_operand_cast = "invalid operand in cast expression";
static string called_object = "called object is not a function";
static string invalid_arguments = "invalid arguments to called function";



/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  Any
 *		previous declaration is discarded.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (funcdefns.count(name) > 0) {
	report(redefined, name);
	outermost->remove(name);
	delete symbol->type().parameters();
	delete symbol;
    }

    symbol = declareFunction(name, type);
    funcdefns.insert(name);
    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		previous declaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (type != symbol->type())
	    report(conflicting, name);

	if (symbol->type().isFunction())
	    delete symbol->type().parameters();

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    outermost->insert(symbol);
    return symbol;
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		previous declaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol != nullptr) {
	if (outermost != toplevel)
	    report(redeclared, name);
	else if (type != symbol->type())
	    report(conflicting, name);

	if (symbol->type().isFunction())
	    delete symbol->type().parameters();

	toplevel->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, type);
    toplevel->insert(symbol);
    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
	report(undeclared, name);
	symbol = new Symbol(name, error);
	toplevel->insert(symbol);
    }

    return symbol;
}


/*
 * Function:	checkFunction
 *
 * Description:	Check if NAME is a previously declared function.  If it is
 *		undeclared, then implicitly declare it.
 */

Symbol *checkFunction(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr)
	symbol = declareFunction(name, Type(INT, 0, nullptr));

    return symbol;
}


Type checkLogicalOR(const Type &left, const Type &right) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isPredicate() && r.isPredicate())
			return integer;
		report(invalid_operands, "||");
	}
	return error;
}


Type checkLogicalAND(const Type &left, const Type &right) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isPredicate() && r.isPredicate())
			return integer;
		report(invalid_operands, "&&");
	}
	return error;
}

Type checkEquality(const Type &left, const Type &right, const string &op) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isCompatibleWith(r))
			return integer;
		report(invalid_operands, op);
	}
	return error;
}

Type checkRelational(const Type &left, const Type &right, const string &op) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isCompatibleWith(r))
			return integer;
		report(invalid_operands, op);
	}
	return error;
}

Type checkRemainder(const Type &left, const Type &right) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l == INT || r == INT)
			return integer;
		report(invalid_operands, "%");
	}
	return error;
}

Type checkAdditive(const Type &left, const Type &right, const string &op) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isNumeric() && r.isNumeric()) {
			if(left == DOUBLE || right == DOUBLE)
				return dbl;
			return integer;
		}
		else if(l.isPointer() && r == INT) {
			return Type(l.specifier(), l.indirection());
		}
		else if(op == "+") {
			if(l == INT && r.isPointer()) {
				if(r.isPointer())
					return r;
				return l;
			}
		}
		else if(op == "-") {
			if((l == r) && (l.isPointer() && r.isPointer()))
				return integer;
		}
		report(invalid_operands, op);
	}
	return error;
}

Type checkMultiplicative(const Type &left, const Type &right, const string &op) {
	Type l = left.promote();
	Type r = right.promote();
	if(!l.isError() && !r.isError()) {
		if(l.isCompatibleWith(r)) {
			if(left == DOUBLE || right == DOUBLE)
				return dbl;
			return integer;
		}
		report(invalid_operands, op);
	}
	return error;
}

Type checkDeref(const Type &right) {
	Type r = right.promote();
	if(!r.isError()) {
		if(r.isPointer())
			return Type(r.specifier(), r.indirection() - 1);
		report(invalid_operand_unary, "*");
	}
	return error;
}

Type checkAddr(const Type &right, const bool &lvalue) {
	Type r = right.promote();
	if(!r.isError()) {
		if(lvalue)
			return Type(r.specifier(), r.indirection() + 1);
		report(lvalue_required);
	}
	return error;
}

Type checkNot(const Type &right) {
	Type r = right.promote();
	if(!r.isError()) {
		if(r.isPredicate())
			return integer;
		report(invalid_operand_unary, "!");
	}
	return error;
}

Type checkNeg(const Type &right) {
	Type r = right.promote();
	if(!r.isError()) {
		if(r.isNumeric())
			return r;
		report(invalid_operand_unary, "-");
	}
	return error;
}

Type checkCast(const Type &result, const Type &right) {
	Type r = right.promote();
	Type res = result.promote();
	
	if(!res.isError() && !r.isError()) {
		if(res.isCompatibleWith(r))
			return result;
		if(res.isPointer() && r.isPointer())
			return result;
		if((res == INT && r.isPointer()) || (res.isPointer() && r == INT))
			return result;
		report(invalid_operand_cast);
	}
	return error;
}

Type checkPostfix(const Type &left, const Type &expression) {
	Type l = left.promote();
	Type expr = expression.promote();
	
	if(!l.isError() && !expr.isError()) {
		if(l.isPointer() && expr == INT)
			return Type(l.specifier(), l.indirection() - 1);
		report(invalid_operands, "[]");
	}
	return error;
}

Type checkPrimaryFunction(const Type &expr, const Parameters &args) {
	if(!expr.isError()) {
		if(!expr.isFunction()) {
			report(called_object);
			return error;
		}
		Parameters *funcParams = expr.parameters();
		if(funcParams != nullptr) {
			if(funcParams->size() == args.size()) {
				for(unsigned i=0; i<args.size(); i++) {
					if(!((*funcParams)[i].isCompatibleWith(args[i].promote()))) {
						report(invalid_arguments);
						return error;
					}	
				}
				return Type(expr.specifier(), expr.indirection());
			}
			else
				report(invalid_arguments);
		}
	}
	return error;
}


void checkReturn(const Type &enclosing, const Type &expr) {
	Type exp = expr.promote();
	
	if(!exp.isError() && !exp.isCompatibleWith(enclosing))
		report(invalid_return);
}

void checkWhileIf(const Type &expr) {
	Type exp = expr.promote();
	
	if(!exp.isError() && !exp.isPredicate())
		report(invalid_type);	
}

void checkAssignment(const Type &left, const Type &right, const bool &lval) {
	Type l = left.promote();
	Type r = right.promote();
	
	if(!l.isError() && !r.isError()) {
		if(!lval)
			report(lvalue_required);
		else if(!l.isCompatibleWith(r))
			report(invalid_operands, "=");
	}
}
