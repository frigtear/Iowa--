#include "parser.h"
#include "token.h"
#include <vector>
#include "errors.h"

Token Parser::peek() const{
    return tokens.at(current);
}

Token Parser::previous() {
    if (current > 0){
        return tokens.at(current - 1);
    }
    return Token("", TokenType::Eof);
}

Token Parser::consume(TokenType expected, const std::string& errorMessage) {
    if (peek().get_type() == expected) {
        Token temp = peek();
        next();
        return temp;
    }
    throw std::runtime_error(errorMessage + 
        " Got “" + Token::get_type_string(peek().get_type()) + "”.");
}

Token Parser::next() {
    if (current < number_of_tokens - 1){
        current++;
        return peek();
    }
    return Token("", TokenType::Eof);
}

bool Parser::is_at_end(){
    return (peek().get_type() == TokenType::Eof);
}

bool Parser::check(TokenType type) const{
    return (peek().get_type() == type);
}

Parser::Parser(std::vector<Token> t) : tokens(t), current(0)
{
    number_of_tokens = t.size();
}

bool Parser::match(std::vector<TokenType> types){
    for (auto type : types){
        if (peek().get_type() == type){
            next();
            return true;
        }
    }
    return false;
}

Declaration* Parser::program(){
    std::vector<Declaration*> declarations;
    while (!is_at_end()){
        declarations.push_back((declaration()));
    }
    return new Program(std::move(declarations));
}

Declaration* Parser::declaration(){
    if (match({TokenType::Set})) {
        return dynamic_declaration();
    }
    else{
        return statement();
    }
}

Declaration* Parser::dynamic_declaration() {

    Token nameToken = consume(TokenType::Identifier, "Expected variable name after 'set'.");
    std::string variable_name = nameToken.get_value();
    consume(TokenType::Equals, "Expected '=' after variable name.");
    Expression* variable_value = expression();
    consume(TokenType::Semicolon,"Expected ';' after variable declaration.");
    return new DynamicDeclaration(variable_name, variable_value);
}

Declaration* Parser::statement(){
    if (match({TokenType::Say})){
        return print_statement();
    }
    else if (match({TokenType::BracketOpen})){
        return block();
    }
    else{
        return expression_statement();
    }
}

Statement* Parser::block(){
    std::vector<Declaration*> statements = {};
    while (!(peek().get_type() == TokenType::BracketClose) && !is_at_end()){
        statements.push_back(declaration());
    }
    consume(TokenType::BracketClose, "Expect '}' after block.");
    return new Block(std::move(statements));
}

Statement* Parser::expression_statement(){
    Expression* value = expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    return new ExpressionStatement(value);
}


Statement* Parser::print_statement(){
    Expression* value = expression();
    consume(TokenType::Semicolon, "Expect ';' after value.");
    return new PrintStatement(value);
}

Expression* Parser::expression() {
    return logic_or();
}

Expression* Parser::logic_or() {
    Expression* expr = logic_and();
    while (match({TokenType::Or})) {
        TokenType op = previous().get_type();
        Expression* right = logic_and();
        expr = new BinaryExpression(expr, op, right);
    }
    return expr;
}

Expression* Parser::logic_and() {
    Expression* expr = equality();
    while (match({TokenType::And})) {
        TokenType op = previous().get_type();
        Expression* right = equality();
        expr = new BinaryExpression(expr, op, right);
    }
    return expr;
}

Expression* Parser::equality() {
    Expression* expr = comparison();
    while (match({TokenType::EqualsEquals, TokenType::NotEqual})) {
        TokenType op = previous().get_type();
        Expression* right = comparison();
        expr = new BinaryExpression(expr, op, right); 
    }
    return expr;
}

Expression* Parser::comparison(){
    Expression* expr = term();
    while (match({TokenType::GreaterThan, TokenType::LessThan})) {
        TokenType op = previous().get_type();
        Expression* right = term();
        expr = new BinaryExpression(expr, op, right); 
    }
    return expr;
}

Expression* Parser::term(){
    Expression* expr = factor();
    while (match({TokenType::Plus, TokenType::Minus})){
        TokenType op = previous().get_type();
        Expression* right = factor();
        expr = new BinaryExpression(expr, op, right); 
    }
    return expr;
}


Expression* Parser::factor() {
    Expression* expr = primary();
    while (match({TokenType::Multiply, TokenType::Divide})) {
        TokenType op = previous().get_type();
        Expression* right = factor(); 
        if (!right) {
            throw std::runtime_error("Missing right-hand side of binary operator");
        }
        expr = new BinaryExpression(expr, op, right);
    }
    return expr;
}


Expression* Parser::primary() {
    if (match({TokenType::Number})) {
        return new Literal(previous().get_value(), TokenType::Number);
    }
    if (match({ TokenType::Boolean })) {
        return new Literal(previous().get_value(), TokenType::Boolean);
    }
    if (match ({TokenType::String })){
        return new Literal(previous().get_value(), TokenType::String);
    }
    if (match ({ TokenType::Identifier })){
        return new Identifier(previous().get_value());
    }
    if (match({TokenType::ParenthesisOpen})) {
        Expression* expr = expression();
        if (!match({TokenType::ParenthesisClose})) {
            throw std::runtime_error("Expected closing parenthesis.");
        }
        return expr;
    }
    throw std::runtime_error("Expressions must be wrapped in parenthesis");
}
