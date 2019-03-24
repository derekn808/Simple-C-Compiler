/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include <string>
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);
Symbol *checkFunction(const std::string &name);

Type checkLogicalOR(const Type &left, const Type &right);
Type checkLogicalAND(const Type &left, const Type &right);
Type checkEquality(const Type &left, const Type &right, const std::string &op);
Type checkRelational(const Type &left, const Type &right, const std::string &op);
Type checkRemainder(const Type &left, const Type &right);
Type checkAdditive(const Type &left, const Type &right, const std::string &op);
Type checkMultiplicative(const Type &left, const Type &right, const std::string &op);
Type checkDeref(const Type &right);
Type checkAddr(const Type &right, const bool &lvalue);
Type checkNot(const Type &right);
Type checkNeg(const Type &right);
Type checkCast(const Type &result, const Type &right);
Type checkPostfix(const Type &left, const Type &expression);
Type checkPrimaryFunction(const Type &expr, const Parameters &args);
void checkReturn(const Type &enclosing, const Type &expr);
void checkWhileIf(const Type &expr);
void checkAssignment(const Type &left, const Type &right, const bool &lval);

# endif /* CHECKER_H */
