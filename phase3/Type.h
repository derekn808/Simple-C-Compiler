#ifndef TYPE_H
#define TYPE_H

# include <vector>
# include <ostream>

typedef std::vector <class Type> Parameters;

class Type {
	int _specifier;
	unsigned _indirection;
	unsigned _length;
	Parameters *_parameters;
	enum {ARRAY, ERROR, FUNCTION, SCALAR} _kind;
	
public:
	Type();
	Type(int specifier, unsigned indirection =0);
	Type(int specifier, unsigned indirection, unsigned length);
	Type(int specifier, unsigned indirection, Parameters *parameters);
	bool operator == (const Type &rhs) const;
	bool operator != (const Type &rhs) const;
	bool isScalar() const;
	bool isArray() const;
	bool isError() const;
	bool isFunction() const;
	int specifier() const;
	unsigned indirection() const;
	unsigned length() const;
	Parameters *parameters() const;	
};
std::ostream &operator<<(std::ostream &ostr, const Type &type);
#endif /* TYPE_H */
