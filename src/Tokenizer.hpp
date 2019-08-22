//
//  Tokenizer.hpp
//  Database
//


#ifndef Tokenizer_hpp
#define Tokenizer_hpp

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "keywords.hpp"


namespace ECE141 {
    
    using callback = bool(char aChar);
    
    //Delineates different types of tokens our system will encounter. You may need to add more...
    enum class TokenType {
        comma=1, colon, semicolon, number, operators, apostrophe,
        lparen, rparen, lbracket, rbracket, lbrace, rbrace,
        slash, star, equal, plus, minus, keyword,
        identifier, function, string, unknown
    };
    
    static std::map<char, TokenType> operatorD = {
        std::make_pair(ECE141::left_paren,       TokenType::lparen),
        std::make_pair(ECE141::right_paren,      TokenType::rparen),
        std::make_pair(ECE141::left_bracket,     TokenType::lbracket),
        std::make_pair(ECE141::right_bracket,    TokenType::rbracket),
        std::make_pair(ECE141::left_brace,       TokenType::lbrace),
        std::make_pair(ECE141::right_brace,      TokenType::rbrace),
        
        std::make_pair(ECE141::colon,            TokenType::colon),
        std::make_pair(ECE141::comma,            TokenType::comma),
        std::make_pair(ECE141::semicolon,        TokenType::semicolon),
        std::make_pair(ECE141::apostrophe,       TokenType::apostrophe),
        std::make_pair('/',                      TokenType::slash),
        std::make_pair('*',                      TokenType::star),
        std::make_pair('=',                      TokenType::equal),
        std::make_pair('+',                      TokenType::plus),
        std::make_pair('-',                      TokenType::minus),
    };
    
    //Holds a given token, created when input (command line or script) is tokenized...
    struct Token {
        Token(TokenType aType=TokenType::unknown,std::string aString="",Keywords aKW=Keywords::unknown_kw) :
        type(aType), keyword(aKW), data(aString) {}
        
        TokenType   type;
        Keywords    keyword;
        std::string data;
    };
    
    
    // USE: This class performs tokenizing for the sytsem --------------------
    
    class Tokenizer {
    public:
        Tokenizer(std::istream &anInputStream);
        
        virtual StatusResult  tokenize();
        virtual Token&        tokenAt(int anOffset);
        size_t                size() {return tokens.size();}
        
        virtual void          restart() {index=0;}
        virtual bool          more() {return index<size();}
        virtual bool          next(int anOffset=1);
        virtual Token&        peek(int anOffset=1);
        virtual Token&        current();
        size_t remaining() {return index<size() ? size()-index :0;}
    protected:
        
        std::vector<Token>    tokens;
        std::istream          &input;
        int                   index;
    };
    
}

#endif /* Tokenizer_hpp */
