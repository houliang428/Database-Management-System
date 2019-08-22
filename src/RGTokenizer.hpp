//
//  RGTokenizer.hpp
//  Database3

#ifndef RGTokenizer_hpp
#define RGTokenizer_hpp

#include "Tokenizer.hpp"
#include "Value.hpp"
#include "Helpers.hpp"

namespace ECE141 {


  class RGTokenizer : public Tokenizer {
  public:
    
                RGTokenizer(std::istream &anInputStream);
    virtual StatusResult  tokenize();
    Token&                tokenAt(int anOffset);

    void            dump();
    
    std::string     readWhile(callback aCallback);
    std::string     readUntil(char aTerminal, bool addTerminal=false);
    std::string     readUntil(callback aCallback, bool addTerminal=false);
    
  };

}

#endif /* RGTokenizer_hpp */
