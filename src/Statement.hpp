//
//  Statement.hpp
//  Database3
//


#ifndef Statement_hpp
#define Statement_hpp

#include "keywords.hpp"
#include <iostream>

namespace ECE141 {
  
  class Tokenizer;
  
  class Statement {
  public:
    Statement(Keywords aStatementType=Keywords::unknown_kw);
    Statement(const Statement &aCopy);
    
    virtual                 ~Statement();
    
    virtual   StatusResult  parse(Tokenizer &aTokenizer);
    
    Keywords                getType() const {return stmtType;}
    virtual   const char*   getStatementName() const {return "statement";}
    virtual   StatusResult  run(std::ostream &aStream) const;
    
  protected:
    Keywords   stmtType;    
  };
  
}

#endif /* Statement_hpp */
