# ifndef CHECKER_H
# define CHECKER_H

# include <string>
# include "Type.h"

typedef std::string string;

void openScope();
void closeScope();
void declareFunction(const string &name, const Type &type);
void declareVariable(const string &name, const Type &type);
void defineFunction(const string &name, const Type &type);
void checkIdentifier(const string &name);
void checkFunction(const string &name);

# endif /* CHECKER_H */
