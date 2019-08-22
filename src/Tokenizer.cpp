//
//  Tokenizer.cpp
//  Database1
//


#include "Tokenizer.hpp"
#include <exception>

namespace ECE141 {
    
    Tokenizer::Tokenizer(std::istream &anInput) : input(anInput), index(0)  {}
    
    //convert input given in constructor to collection of tokens...
    StatusResult Tokenizer::tokenize() {
        StatusResult theResult;
        
        //-----------------------------------------
        // STUDENT: You will implement this method
        //-----------------------------------------
        while(!input.eof()){
            char theChar = input.peek();
            
            //is Operator (one char)
            if(operatorD.count(theChar)){
                Token newToken;
                newToken.type = operatorD[theChar];
                std::string temp;
                input.get(theChar);
                temp += theChar;
                theChar = input.peek();
                
                newToken.data = temp;
                newToken.keyword = Keywords::unknown_kw;
                tokens.push_back(newToken);
              
            }
            
            //is number
            else if(isnumber(theChar)){
                Token newToken;
                newToken.type = TokenType::number;
                std::string temp;
                
                while(isnumber(theChar) || theChar=='.'){
                    input.get(theChar);
                    temp+=theChar;
                    theChar = input.peek();
                }
                
                newToken.data = temp;
                newToken.keyword = Keywords::unknown_kw;
                tokens.push_back(newToken);
                //std::cout<<"Number: "<<temp<<std::endl;
            }
            
            //is alpha
            else if(isalpha(theChar)){
                Token newToken;
                std::string temp;
                
                while(isalnum(theChar) || theChar =='_'){
                    input.get(theChar);
                    temp+=theChar;
                    theChar = input.peek();
                }
                
                //is keyword
                if(gDictionary.count(temp)){
                    newToken.keyword = gDictionary[temp];
                    newToken.type = TokenType::keyword;
                    //std::cout<<"keyword: "<<temp<<std::endl;
                }
                
                //is identifier
                else{
                    newToken.keyword = Keywords::unknown_kw;
                    newToken.type = TokenType::identifier;
                    //std::cout<<"identifier: "<<temp<<std::endl;
                }
                
                newToken.data = temp;
                tokens.push_back(newToken);
            }
            
            //else
            else  input.get(theChar);
        }
        
        return theResult;
    }
    
    Token& Tokenizer::tokenAt(int anOffset) {
        if(anOffset<size()) return tokens[anOffset];
        throw std::range_error("bad offset");
    }
    
    bool Tokenizer::next(int anOffset) {
        index += anOffset;
        return true;
    }
    
    Token& Tokenizer::current() {
        return tokens[index];
    }
    
    Token& Tokenizer::peek(int anOffset) {
        return tokenAt(index+anOffset);
    }
    
}
