# ifndef SYMBOL_H
# define SYMBOL_H
# include <string>
# include "Type.h"

typedef std::string string;

class Symbol {
	string _name;
	Type _type;

public:
	Symbol(const string &name, const Type &type);
	const string name() const;
	const Type type() const;
};

# endif /* SYMBOL_H */
