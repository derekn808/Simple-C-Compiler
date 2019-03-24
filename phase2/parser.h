# ifndef PARSER_H
# define PARSER_H

void error();
void match();
void expression();
void and_expression();
void equality_expression();
void comparison_expression();
void additive_expression();
void multiplicative_expression();
void unary_expression();
void array_expression(bool lp);
void primary_expression(bool lp);
void expression_list();
bool is_specifier(int token);
void specifier();
void pointers();
void statement();
void statements();
void declarator();
void declaration();
void declarator_list();
void declarations();
void func_or_global();
void remaining_decls();
void global_declarator();
void parameters();
void parameter();
void parameter_list();

# endif /* LEXER_H */
