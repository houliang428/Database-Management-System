//
//  CommandProcessor.hpp
//  Database3


#ifndef CommandProcessor_hpp
#define CommandProcessor_hpp

#include <stdio.h>
#include <string>

#include "keywords.hpp"

namespace ECE141 {
  
  class Statement;
  class Tokenizer;
  class Database;
  
  class Command {
  public:
    Command(Keywords aKeyword) : keyword(aKeyword) {}
    
    Keywords getKeyword() const {return keyword;}
    
  protected:
    Keywords keyword;
  };  
  
  //--------------------------------------------------
  
  class IInterpreter {
  public:
    virtual StatusResult  processInput(Tokenizer &aTokenizer)=0;
    virtual StatusResult  interpret(const Statement &aStatement)=0;
    virtual Statement*    getStatement(Tokenizer &aTokenizer)=0;
    virtual Database*     getActiveDatabase()=0;
  };
  
  //--------------------------------------------------
  
  class CommandProcessor : public IInterpreter {
  public:
    
    CommandProcessor(IInterpreter *aNext=nullptr);
    virtual ~CommandProcessor();
    
    virtual StatusResult  processInput(Tokenizer &aTokenizer);
    virtual Database*     getActiveDatabase();

  protected:
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);

    IInterpreter *next;
    
  };
}

#endif /* CommandProcessor_hpp */
