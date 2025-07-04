#ifndef PARSER_H
#define PARSER_H

#include "expressions.h"
#include "token.h"
#include <vector>

class Parser{
    public:
        Parser(std::vector<Token>* t) : tokens(t) {}

        Token peek();
        Token previous();
        Token next();

        bool match(std::vector<std::string> types);
        bool match(std::vector<TokenType> types);    

        Expression* expression();
        Expression* equality();
        Expression* comparison();
        Expression* term();
        Expression* primary();
        Expression* factor();
        Expression* assignment();
        Expression* expression_statement();
        Expression* if_statement();
        Expression* literal();
        Expression* identifier();
        Expression* number();
        Expression* string_literal(); 
        Expression* equality_operator();
        Expression* comparison_operator();
        

    private: 
        std::vector<Token>* tokens;
        int current = 0;
};

#endif