/*
 * File:	generator.cpp
 *
 * Description:	This file contains the public and member function
 *		definitions for the code generator for Simple C.
 *
 *		Extra functionality:
 *		- putting all the global declarations at the end
 */

# include <sstream>
# include <iostream>
# include "generator.h"
# include "Label.h"
# include "machine.h"
# include "Tree.h"
# include "Register.h"

using namespace std;


/* This needs to be set to zero if temporaries are placed on the stack. */

# define SIMPLE_PROLOGUE 0


/* This should be set if we want to use the callee-saved registers. */

# define CALLEE_SAVED 0

# define FP(expr) ((expr)->type().isReal())
# define BYTE(expr) ((expr)->type().size() == 1)

/* The registers and their related functions */

typedef vector<Register *>Registers;

static Register *ebx = new Register("%ebx", "%bl");
static Register *esi = new Register("%esi");
static Register *edi = new Register("%edi");

string returnLabel;
Register *eax = new Register("%eax", "%al");
Register *ecx = new Register("%ecx", "%cl");
Register *edx = new Register("%edx", "%dl");
vector<Register *> registers{eax, ecx, edx};
Register *mm0 = new Register("%xmm0", "%al");
Register *mm1 = new Register("%xmm1", "%al");
Register *mm2 = new Register("%xmm2", "%al");
Register *mm3 = new Register("%xmm3", "%al");
Register *mm4 = new Register("%xmm4", "%al");
Register *mm5 = new Register("%xmm5", "%al");
Register *mm6 = new Register("%xmm6", "%al");
Register *mm7 = new Register("%xmm7", "%al");
vector<Register *> fp_registers{mm0, mm1, mm2, mm3, mm4, mm5, mm6, mm7};
int offset = 0;

# if CALLEE_SAVED
static Registers callee_saved = {ebx, esi, edi};
# else
static Registers callee_saved = {};
# endif

/*
 * Function:	generateGlobals
 *
 * Description:	Generate code for any global variable declarations.
 */

void generateGlobals(Scope *scope)
{
    const Symbols &symbols = scope->symbols();
	
    for (unsigned i = 0; i < symbols.size(); i ++)
	if (!symbols[i]->type().isFunction()) {
	    cout << "\t.comm\t" << global_prefix << symbols[i]->name() << ", ";
	    cout << symbols[i]->type().size() << endl;
	}
}

/*
 * Function:	operator << (private)
 *
 * Description:	Write an expression to the specified stream.  This function
 *		first checks to see if the expression is in a register, and
 *		if not then uses its operand.
 */

static ostream &operator <<(ostream &ostr, Expression *expr)
{
    if (expr->_register == nullptr)
		return ostr << expr->_operand;
	unsigned size = expr->type().size();
	return ostr << expr->_register->name(size);
}

static string suffix(Expression *expr) {
	return FP(expr) ? "sd\t" : (BYTE(expr) ? "b\t" : "l\t");
}

/*
static void compareZero(Expression *expr) {
	expr->generate();
	cout << "\t#COMPAREZERO" << endl;
	if(FP(expr)) {
		if(expr->_register == nullptr)
			load(expr, fp_getreg());
		
		Register *reg = fp_getreg();
		cout << "\tpxor\t" << reg << ", " << reg << endl;
		cout << "\tucomsid\t" << reg << ", " << expr << endl;
		//assign(expr, nullptr);
		assign(nullptr, reg);
	}
	else {
		if(expr->_register == nullptr)
			load(expr, getreg());
		cout << "\tcmpl\t$0, " << expr << endl;
		assign(expr, nullptr);
	}
}
*/
Register *fp_getreg() {
	for(unsigned i = 0; i < fp_registers.size(); i++) {
		if(fp_registers[i]->_node == nullptr)
			return fp_registers[i];
	}
	
	load(nullptr, fp_registers[0]);
	return fp_registers[0];
}

Register *getreg() {
	for(unsigned i = 0; i < registers.size(); i++) {
		if(registers[i]->_node == nullptr)
			return registers[i];
	}
	
	load(nullptr, registers[0]);
	return registers[0];
}

void assign(Expression *expr, Register *reg) {
	//cout << "\t#ASSIGN" << endl;
	if(expr != nullptr) {
		if(expr->_register != nullptr)
			expr->_register->_node = nullptr;
		expr->_register = reg;
	}
	
	if(reg != nullptr) {
		if(reg->_node != nullptr)
			reg->_node->_register = nullptr;
		reg->_node = expr;
	}
}

void release() {
	for(unsigned i = 0; i < registers.size(); i++)
		assign(nullptr, registers[i]);
}

void assigntemp(Expression *expr) {
	stringstream ss;
	
	offset = offset - expr->type().size();
	ss << offset << "(%ebp)";
	expr->_operand = ss.str();
}

void load (Expression *expr, Register *reg) {
	//cout << "\t#LOAD" << endl;
	if(reg->_node != expr) {
		if(reg->_node != nullptr) {
			unsigned size = reg->_node->type().size();
			assigntemp(reg->_node);
			cout << "\tmov" << suffix(reg->_node);
			cout << reg->name(size) << ", ";
			cout << reg->_node->_operand << endl;
		}
		
		if(expr != nullptr) {
			unsigned size = expr->type().size();
			cout << "\tmov" << suffix(expr) << expr;
			cout << ", " << reg->name(size) << endl;
		}
		assign(expr, reg);
	}
}

/*
 * Function:	align (private)
 *
 * Description:	Return the number of bytes necessary to align the given
 *		offset on the stack.
 */

static int align(int offset)
{
    if (offset % STACK_ALIGNMENT == 0)
	return 0;

    return STACK_ALIGNMENT - (abs(offset) % STACK_ALIGNMENT);
}


/*
 * Function:	Identifier::generate
 *
 * Description:	Generate code for an identifier.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void Identifier::generate()
{
	//cout << "\t#ID" << endl;
    stringstream ss;


    if (_symbol->_offset != 0)
	ss << _symbol->_offset << "(%ebp)";
    else
	ss << global_prefix << _symbol->name();

    _operand = ss.str();
}


/*
 * Function:	Integer::generate
 *
 * Description:	Generate code for an integer.  Since there is really no
 *		code to generate, we simply update our operand.
 */

void Integer::generate()
{
	//cout << "\t#INT" << endl;
    stringstream ss;

    ss << "$" << _value;
    _operand = ss.str();
}

void String::generate() {
	//cout << "\t#STRING" << endl;
	stringstream ss;
	Label ll;
	ss << ll;
	_operand = ss.str();
	cout << "\t.data" << endl;
	cout << _operand << ":\t.asciz " << value() << "\n\t.text" << endl;
	//assign(this, getreg());
	//cout << "\tleal\t" << _operand << ", " << _register << endl;
}

void Real::generate() {
	//cout << "\t#REAL" << endl;
	stringstream ss;
	Label ll;
	ss << ll;
	_operand = ss.str();
	cout << "\t.data" << endl;
	cout << _operand << ":\t.double " << value() << "\n\t.text" << endl;
}

/*
 * Function:	Call::generate
 *
 * Description:	Generate code for a function call expression.
 *
 *		NOT FINISHED: Only guaranteed to work if the argument is
 *		either an integer literal or an integer scalar variable.
 */

void Call::generate()
{
    unsigned bytesPushed = 0;
	cout << "\t#CALL" << endl;

    /* Compute how many bytes will be pushed on the stack. */

    for (int i = _args.size() - 1; i >= 0; i --) {
	bytesPushed += _args[i]->type().size();
	
	if (STACK_ALIGNMENT != 4 && _args[i]->_hasCall)
		_args[i]->generate();
	}


    /* Adjust the stack keep it aligned.  We do this by simply subtracting
       the necessary number of bytes from the stack pointer.  Effectively,
       we are pushing nonexistent arguments on the stack so that the total
       number of bytes pushed is a multiple of the alignment. */

    if (align(bytesPushed) > 0) {
	cout << "\tsubl\t$" << align(bytesPushed) << ", %esp" << endl;
	bytesPushed += align(bytesPushed);
    }


    /* Push each argument on the stack. */

    /*for (int i = _args.size() - 1; i >= 0; i --) {
	if (STACK_ALIGNMENT == 4 || !_args[i]->_hasCall)
	    _args[i]->generate();

	cout << "\tpushl\t" << _args[i] << endl;
    }
	*/

	for(int i = _args.size() - 1; i>=0; i --) {
		if (STACK_ALIGNMENT == 4 || !_args[i]->_hasCall)
			_args[i]->generate();
		if(FP(_args[i])) {
			cout << "\tmovsd\t" << _args[i] << ", " << mm0 << endl;
			cout << "\tsubl\t$8, %esp" << endl;
			cout << "\tmovsd\t" << mm0 << ", (%esp)" << endl;
		}
		else {
			cout << "\tpushl\t" << _args[i] << endl;
		}
	}

	//spill registers load nullptr
	for(unsigned i = 0; i < registers.size(); i++) {
		load(nullptr, registers[i]);
	}
	for(unsigned i = 0; i < fp_registers.size(); i++) {
		load(nullptr, fp_registers[i]);
	}
    /* Call the function and then adjust the stack pointer back. */

    cout << "\tcall\t" << global_prefix << _id->name() << endl;
	
	if(FP(this)) {
		assigntemp(this);
		cout << "\tfstpl\t" << this << endl;
	}
	else
		assign(this, eax);
		
	if (bytesPushed > 0)
		cout << "\taddl\t$" << bytesPushed << ", %esp" << endl;
}


/*
 * Function:	Block::generate
 *
 * Description:	Generate code for this block, which simply means we
 *		generate code for each statement within the block.
 */

void Block::generate()
{
    for (unsigned i = 0; i < _stmts.size(); i ++){
		_stmts[i]->generate();
		release();
	}
}


/*
 * Function:	Function::generate
 *
 * Description:	Generate code for this function, which entails allocating
 *		space for local variables, then emitting our prologue, the
 *		body of the function, and the epilogue.
 *
 *		The stack must be aligned at the point at which a function
 *		begins execution.  Since the call instruction pushes the
 *		return address on the stack and each function is expected
 *		to push its base pointer, we adjust our offset by that
 *		amount and then perform the alignment.
 *
 *		On a 32-bit Intel platform, 8 bytes are pushed (4 for the
 *		return address and 4 for the base pointer).  Since Linux
 *		requires a 4-byte alignment, all we need to do is ensure
 *		the stack size is a multiple of 4, which will usually
 *		already be the case.  However, since OS X requires a
 *		16-byte alignment (thanks, Apple, for inventing your own
 *		standards), we will often see an extra amount of stack
 *		space allocated.
 *
 *		On a 64-bit Intel platform, 16 bytes are pushed (8 for the
 *		return address and 8 for the base pointer).  Both Linux and
 *		OS X require 16-byte alignment.
 */

void Function::generate()
{
	stringstream ss;
	ss << _id->name() << ".exit";
	returnLabel = ss.str();

    int param_offset;

	//cout << "\t#FUNCGENERATE" << endl;
    /* Generate our prologue. */

    param_offset = PARAM_OFFSET + SIZEOF_REG * callee_saved.size();
    offset = param_offset;
    allocate(offset);

    cout << global_prefix << _id->name() << ":" << endl;
    cout << "\tpushl\t%ebp" << endl;

    for (unsigned i = 0; i < callee_saved.size(); i ++)
	cout << "\tpushl\t" << callee_saved[i] << endl;

    cout << "\tmovl\t%esp, %ebp" << endl;

    if (SIMPLE_PROLOGUE) {
	offset -= align(offset - param_offset);
	cout << "\tsubl\t$" << -offset << ", %esp" << endl;
    } else
	cout << "\tsubl\t$" << _id->name() << ".size, %esp" << endl;

	
	
    /* Generate the body of this function. */

    _body->generate();

	cout << returnLabel << ":" << endl;

    /* Generate our epilogue. */

    cout << "\tmovl\t%ebp, %esp" << endl;

    for (int i = callee_saved.size() - 1; i >= 0; i --)
	cout << "\tpopl\t" << callee_saved[i] << endl;

    cout << "\tpopl\t%ebp" << endl;
    cout << "\tret" << endl << endl;

    if (!SIMPLE_PROLOGUE) {
	offset -= align(offset - param_offset);
	cout << "\t.set\t" << _id->name() << ".size, " << -offset << endl;
    }

    cout << "\t.globl\t" << global_prefix << _id->name() << endl << endl;
}

/*Generate functions*/
/*
 * Function:	Assignment::generate
 *
 * Description:	Generate code for an assignment statement.
 *
 *		NOT FINISHED: Only guaranteed to work if the right-hand
 *		side is an integer literal and the left-hand side is an
 *		integer scalar.
 */

void Assignment::generate()
{
    _right->generate();
	
	cout << "\t#ASSIGNMENT" << endl;
	
	Expression *child = _left->isDeref();
	if(child != nullptr) {
		child->generate();
		if(child->_register == nullptr)
			load(child, FP(child) ? fp_getreg() : getreg());
		if(_right->_register == nullptr)
			load(_right, FP(_right) ? fp_getreg() : getreg());
		cout << "\tmov" << suffix(_right) <<_right << ", (" << child << ")" << endl;
		assign(_left, child->_register);
	}	
	else {
		_left->generate();
		if(_right->_register == nullptr)
			load(_right, FP(_right) ? fp_getreg() : getreg());
		cout << "\tmov" << suffix(_left) <<_right << ", " << _left << endl;
	}
	assign(_right, nullptr);
}

//Arithmetic
void Add::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#ADD" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	cout << "\tadd" << suffix(_left);
	cout << _right << ", " << _left << endl;
	
	assign(_right, nullptr);
	assign(this, _left->_register);
}

void Subtract::generate() {
	_left->generate();
	_right->generate();

	cout << "\t#SUBTRACT" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	cout << "\tsub" << suffix(_left);
	cout << _right << ", " << _left << endl;
	
	assign(_right, nullptr);
	assign(this, _left->_register);
}

void Multiply::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#MULTIPLY" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tmulsd\t" << _right << ", " << _left << endl;
	}
	else {
		cout << "\timul" << suffix(_left);
		cout << _right << ", " << _left << endl;
	}
	
	assign(_right, nullptr);
	assign(this, _left->_register);
}

void Divide::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#DIVIDE" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {	
		cout << "\tdivsd\t";
		cout << _right << ", " << _left << endl;
	
		assign(_right, nullptr);
		assign(this, _left->_register);
	}
	
	else {
		load(_left, eax);
		load(_right, ecx);
		load(nullptr, edx);
		
		cout << "\tcltd" << endl;
		cout << "\tidivl\t%ecx" << endl;
		
		assign(nullptr, eax);
		assign(nullptr, ecx);
		assign(nullptr, edx);
		assign(this, eax);
	}
}

void Remainder::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#REMAINDER" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
		
	if(FP(_left)) {	
		cout << "\tdivsd\t";
		cout << _right << ", " << _left << endl;
	
		assign(_right, nullptr);
		assign(this, _left->_register);
	}
	
	else {
		load(_left, eax);
		load(_right, ecx);
		load(nullptr, edx);
		
		cout << "\tcltd" << endl;
		cout << "\tidivl\t%ecx" << endl;
		
		assign(nullptr, eax);
		assign(nullptr, ecx);
		assign(nullptr, edx);
		assign(this, edx);
	}
}

//Binary
void LessThan::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#LESSTHAN" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetb\t" << _register->byte() << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetl\t" << _register->byte() << endl;
	}
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void GreaterThan::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#GREATERTHAN" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tseta\t" << _register->byte() << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetg\t" << _register->byte() << endl;
	}
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void LessOrEqual::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#LESSEQUAL" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetbe\t" << _register->byte() << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetle\t" << _register->byte() << endl;
	}
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void GreaterOrEqual::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#GREATEREQUAL" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetae\t" << _register->byte() << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
		assign(_right, nullptr);
		assign(_left, nullptr);
		assign(this, getreg());
		cout << "\tsetge\t" << _register->byte() << endl;
	}
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void NotEqual::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#NOT EQUAL" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
	}
	assign(_right, nullptr);
	assign(_left, nullptr);
	assign(this, getreg());
	cout << "\tsetne\t" << _register->byte() << endl;
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void Equal::generate() {
	_left->generate();
	_right->generate();
	
	cout << "\t#EQUAL" << endl;
	if(_left->_register == nullptr)
		load(_left, FP(_left) ? fp_getreg() : getreg());
	
	if(FP(_left)) {
		cout << "\tucomisd\t" << _right << ", " << _left << endl;
	}
	
	else {
		cout << "\tcmpl\t" << _right << ", " << _left << endl;
	}
	assign(_right, nullptr);
	assign(_left, nullptr);
	assign(this, getreg());
	cout << "\tsete\t" << _register->byte() << endl;
	cout << "\tmovzbl\t" << _register->byte() << ", " << this << endl;
}

void LogicalOr::generate() {
cout << "\t#OR" << endl;
	Label onTrue, skip;
	_left->test(onTrue, true);
	_right->test(onTrue, true);
	assign(this, getreg());
	cout << "\tmovl\t$0, " << this << endl;
	cout << "\tjmp\t" << skip << endl;
	cout << onTrue << ":" << endl;
	cout << "\tmovl\t$1, " << this << endl;
	cout << skip << ":" << endl;
}

void LogicalAnd::generate() {
	cout << "\t#AND" << endl;
	Label onTrue, skip;
	_left->test(onTrue, false);
	_right->test(onTrue, false);
	assign(this, getreg());
	cout << "\tmovl\t$1, " << this << endl;
	cout << "\tjmp\t" << skip << endl;
	cout << onTrue << ":" << endl;
	cout << "\tmovl\t$0, " << this << endl;
	cout << skip << ":" << endl;
}

//Unary

void Negate::generate() {
	_expr->generate();
	cout << "\t#NEGATE" << endl;
	if(_expr->_register == nullptr)
		load(_expr, FP(_expr) ? fp_getreg() : getreg());
	if(FP(_expr)) {
		cout << "\tcmpl\t$0, " << _expr->_register << endl;
		cout << "\tsete\t" << _expr->_register->byte() << endl;
		cout << "\tmovzbl\t" << _expr->_register->byte() << ", " << _expr->_register << endl;
	}
	else {
		cout << "\tnegl\t" << _expr->_register << endl;
		assign(this, _expr->_register);
	}
}

void Not::generate() {
	_expr->generate();
	cout << "\t#NOT" << endl;
	if(_expr->_register == nullptr) {
		load(_expr, FP(_expr) ? fp_getreg() : getreg());
	}
	if(FP(_expr)) {
		Register *reg = fp_getreg();
		assign(this, getreg());
		cout << "\tpxor\t" << reg << ", " << reg << endl;
		cout << "\tucomsid\t" << reg << ", " << _expr << endl;
		cout << "\tsete\t" <<  _expr->_register->byte() << endl;
		cout << "\tmovzbl\t" <<  _expr->_register->byte() << ", " <<  _expr->_register << endl;
		assign(_expr, nullptr);
		assign(nullptr, reg);
	}
	else {
		cout << "\tcmpl\t$0, " << _expr << endl;
		cout << "\tsete\t" <<  _expr->_register->byte() << endl;
		cout << "\tmovzbl\t" <<  _expr->_register->byte() << ", " << _expr->_register << endl;
		assign(this, _expr->_register);
	}
	
}

void Address::generate() {
	cout << "\t#ADDRESS" << endl;
	Expression *child = _expr->isDeref();
	if(child != nullptr) {
		child->generate();
		_operand = child->_operand;
		assign(this, child->_register);
	}
	else {
		_expr->generate();
		assign(this, getreg());
		cout << "\tleal\t" << _expr->_operand << ", " << _register << endl;
	}
}

void Dereference::generate() {
	_expr->generate();
	cout << "\t#DEREF" << endl;
	
	if(_expr->_register == nullptr)
		load(_expr, getreg());
	cout << "\tmov" << suffix(this) << "(" << _expr->_register << "), ";
	assign(_expr, nullptr);
	assign(this, FP(_expr) ? fp_getreg() : getreg());
	cout  << this << endl;
}

void Cast::generate() {
	_expr->generate();
	cout << "\t#CAST" << endl;
	if(_expr->_register == nullptr)
		load(_expr, FP(_expr) ? fp_getreg() : getreg());
	//cast expression
	//	(type) expression
	
	Type src = _expr->type();
	Type dest = _type;
	//"source" = expression
	//"destination" = type
	if(src.size()==4) {
		if(dest.size() == 8) {
			//int/pointer ->double
			assign(this, fp_getreg());
			cout << "\tcvtsi2sd\t" << _expr << ", " << this << endl;
			assign(_expr, nullptr);
		}
		else if(dest.size() == 4) {
			//int/pointer -> int/pointer
			assign(this, _expr->_register);
		}
		else {
			//int/pointer -> char
			assign(this, _expr->_register);
		}
	}
	else if (src.size() == 8) {
		if(dest.size() == 8) {
			//double->double
			assign(this, _expr->_register);
		}
		else if(dest.size() == 4) {
			//double -> int
			assign(this, getreg());
			cout << "\tcvttsd2si\t" << _expr << ", " << this << endl;
			assign(_expr, nullptr);
		}
		else {
			//double -> char
			assign(this, getreg());
			cout << "\tcvttsd2si\t" << _expr << ", " << this << endl;
			assign(_expr, nullptr);
		}
	}
	else {
		if(dest.size() == 8) {
			//char->double
			assign(this, fp_getreg());
			cout << "\tmovsbl\t" << _expr << ", " << _expr->_register->name() << endl;
			cout << "\tcvtsi2sd\t" << _expr->_register->name() << ", " << this << endl;
			assign(_expr, nullptr);
		}
		else if(dest.size() == 4) {
			//char -> int
			cout << "\tmovsbl\t" << _expr->_register->name() << ", " << this << endl;
			assign(this, _expr->_register);
		}
		else {
			//char -> char
			assign(this, _expr->_register);
		}
	}
}

//Control Flow
void While::generate() {
	Label loop, exit;
	cout << "\t#LOOP" << endl;
	cout << loop << ":" << endl;
	
	_expr->test(exit, false);
	_stmt->generate();
	release();
	
	cout << "\tjmp\t" << loop << endl;
	cout << exit << ":" << endl;
}

void Return::generate() {
	_expr->generate();
	cout << "\t#RETURN" << endl;
	
	if(FP(_expr)) {
		if(_expr->_register != nullptr)
			load(nullptr, _expr->_register);
		cout << "\tfldl\t" << _expr << endl;
	}
	else {
		load(_expr, eax);
	}
	cout << "\tjmp\t" << returnLabel << endl;
}

//use expr->test()
void If::generate() {
	//_expr->generate();
	cout << "\t#IF" << endl;
	Label skip, then;
	
	_expr->test(then, false);
	_thenStmt->generate();
	release();
	if(_elseStmt != nullptr) {
		cout << "\tjmp\t" << skip << endl;
		cout << then << ":" << endl;
		_elseStmt->generate();
	}
	else
		cout <<  then << ": " << endl;
	cout << skip << ":" << endl;
}

//Test Functions
void Expression::test(const Label &label, bool ifTrue) {
	//compareZero(this);

	generate();
	if(FP(this)) {
		if(_register == nullptr) 
			load(this, fp_getreg());
		Register *reg = fp_getreg();
		cout << "\tpxor\t" << reg << ", " << reg << endl;
		cout << "\tucomsid\t" << reg << ", " << this << endl;
		assign(nullptr, reg);
	}
	else {
		if(_register == nullptr) 
			load(this, getreg());
		cout << "\tcmpl\t$0, " << this << endl;
	}
	cout << (ifTrue ? "\tjne\t" : "\tje\t") << label << endl;
	
	assign(this, nullptr);
}


void LessThan::test(const Label &label, bool ifTrue) {
	_left->generate();
	_right->generate();
	
	if(_left->_register == nullptr)
		load(_left, getreg());
		
	cout << "\tcmpl\t" << _right << ", " << _left << endl;
	cout << (ifTrue ? "\tjl\t" : "\tjge\t") << label << endl;
	
	assign(_left, nullptr);
	assign(_right, nullptr);
}
