# include <iostream>
# include "generator.h"
# include "Tree.h"
# include "Type.h"

using std::cout;
using std::endl;

void generateGlobals(const Scope *global) {
	const Symbols &globalSym = global->symbols();
	for(unsigned i = 0; i < globalSym.size(); i++) {
		if(!globalSym[i]->type().isFunction()) {
			cout << ".comm\t" << globalSym[i]->name();
			cout << ", " << globalSym[i]->type().size();
			cout << ", " << globalSym[i]->type().size() << endl;
		}
	}	
}

void Function::generate() {
	Parameters *params = _id->type().parameters();
	Symbols funcSymbols = _body->declarations()->symbols();
	
	for(unsigned i = 0; i < params->size(); i++) {
		funcSymbols[i]->setOffset(8+i*4);
	}
	
	int offset = 0;
	for(unsigned i = params->size(); i < funcSymbols.size(); i++) {
		offset -= funcSymbols[i]->type().size();
		funcSymbols[i]->setOffset(offset);
	}
	
	//prologue
	cout << ".globl\t" << _id->name() << endl;
	cout << _id->name() << ":" << endl;
	cout << "\tpushl\t%ebp\n" << endl;
	cout << "\tmovl\t%esp, %ebp\n" << endl;
	cout << "\tsubl\t$" << -offset << ", %esp\n" << endl;
	
	_body->generate();

	//epilogue
	cout << "\tmovl\t" << "%ebp, %esp\n" << endl;
	cout << "\tpopl\t%ebp\n" << endl;
	cout << "\tret\n" << endl;
}

void Block::generate() {
	for(unsigned i = 0; i < _stmts.size(); i++)
		_stmts[i]->generate();
}

void Assignment::generate() {
    cout << "\tmovl\t";
    _right->generate(); 
    cout << ", %eax" << endl;
    cout << "\tmovl\t%eax, ";
    _left->generate();
    cout << endl;
}

void Call::generate() {
	for(auto i = _args.end()-1; i >=_args.begin(); i--) {
		cout << "\tpushl\t";
		(*i)->generate();
		cout << endl;
	}
	cout << "\tcall\t" << _id->name() << endl;
}

void Identifier::generate() {
	if(_symbol->getOffset() != 0)
		cout << _symbol->getOffset() << "(%ebp)" << endl;
	else
		cout << _symbol->name() << endl;
}

void Integer::generate() {
	cout << "$" << _value;
}
