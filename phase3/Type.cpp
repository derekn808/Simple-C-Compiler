# include "Type.h"
# include <cassert>

using namespace std;

Type::Type(int specifier, unsigned indirection, Parameters *parameters)
	:_specifier(specifier), _indirection(indirection), _parameters(parameters), _kind(FUNCTION)
{
}	

Type::Type()
	: _kind(ERROR)
{
}

Type::Type(int specifier, unsigned indirection)
	: _specifier(specifier), _indirection(indirection), _kind(SCALAR)
{
}

Type::Type(int specifier, unsigned indirection, unsigned length)
	: _specifier(specifier), _indirection(indirection), _length(length)
{
	_kind = ARRAY;
}

bool Type::operator !=(const Type &rhs) const {
	return !operator == (rhs);
}

bool Type::operator ==(const Type &rhs) const {
	if(_kind != rhs._kind)
		return false;
	if(_kind == ERROR)
		return true;
	if(_specifier != rhs._specifier)
		return false;
	if(_indirection != rhs._indirection)
		return false;
	if(_kind == SCALAR)
		return true;
	if(_kind == ARRAY)
		return _length == rhs._length;
	if(!_parameters || !rhs._parameters)
		return true;
	return *_parameters == *rhs._parameters;
}

bool Type::isScalar() const {
	return _kind == SCALAR;
}

bool Type::isArray() const {
	return _kind == ARRAY;
}

bool Type::isError() const {
	return _kind == ERROR;
}

bool Type::isFunction() const {
	return _kind == FUNCTION;
}

int Type::specifier() const {
	return _specifier;
}

unsigned Type::indirection() const {
	return _indirection;
}

unsigned Type::length() const {
	assert(_kind == ARRAY);
	return _length;
}

Parameters *Type::parameters() const {
	assert(_kind == FUNCTION);
	return _parameters;
}
