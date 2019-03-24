/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include <string>
# include <ostream>
# include "Scope.h"
# include "Tree.h"
# include "Register.h"

void generateGlobals(Scope *scope);

Register *fp_getreg();
Register *getreg();
void assign(Expression *expr, Register *reg);
void release();
void assigntemp(Expression *expr);
void load (Expression *expr, Register *reg);


# endif /* GENERATOR_H */
