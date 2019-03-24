# include <cassert>
# include "Scope.h"

Scope::Scope(Scope *enclosing)
	: _enclosing(enclosing)
{
}

void Scope::insert(Symbol *symbol) {
	assert(find(symbol->name()) == nullptr);
	_symbols.push_back(symbol);
}

void Scope::remove(const string &name) {
	for(unsigned i=0; i< _symbols.size(); i++) {
		if(name == _symbols[i]->name()) {
			_symbols.erase(_symbols.begin() + i);
		}
	}
}

Symbol *Scope::find(const string &name) const {
	for(unsigned i =0; i<_symbols.size(); i++) {
		if(name == _symbols[i]->name()) {
			return _symbols[i];
		}
	}
	
	return NULL;
}

Symbol *Scope::lookup(const string &name) const {
	Symbol *symbol;

	if((symbol = find(name)) != nullptr) {
		return symbol;
	}

	else if(_enclosing != nullptr) {
		return _enclosing->lookup(name);
	}

	else {
		return NULL;
	}
}

Scope *Scope::enclosing() const {
	return _enclosing;
}

const Symbols &Scope::symbols() const {
	return _symbols;
}
