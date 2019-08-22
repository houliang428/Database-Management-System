//
//  CommandProcessor.cpp
//  Database3
//

#include <iostream>
#include "CommandProcessor.hpp"
#include "Statement.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  CommandProcessor::CommandProcessor(IInterpreter *aNext) : next(aNext) {}
  
  CommandProcessor::~CommandProcessor() {}
  
  // USE: -----------------------------------------------------
  StatusResult CommandProcessor::interpret(const Statement &aStatement) {
    switch(aStatement.getType()) {
      case Keywords::quit_kw:
        std::cout << "DB::141 is shutting down." << std::endl;
        return StatusResult(ECE141::userTerminated);
        break;
      case Keywords::version_kw: std::cout << "Version 0.8" << std::endl; break;
      case Keywords::help_kw: std::cout << "Show help here..." << std::endl; break;
      default: break;
    }
    return StatusResult();
  }
  
  // USE: retrieve a statement based on given text input...
  Statement* CommandProcessor::getStatement(Tokenizer &aTokenizer) {
    
    if(aTokenizer.more()) {
      Token &theToken = aTokenizer.current();
      switch(theToken.keyword) {
        case Keywords::help_kw    :
        case Keywords::version_kw :
        case Keywords::quit_kw    :
          aTokenizer.next(); //skip ahead...
          return new Statement(theToken.keyword);
          
        default: break;
      }
    }
    return nullptr;
  }
  
  // USE: parse tokens in tokenizer and create assocated statement;
  StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer) {
    std::unique_ptr<Statement> theStatement(getStatement(aTokenizer));
    if(theStatement) {
      return interpret(*theStatement);
    }
    else if(next) {
      return next->processInput(aTokenizer);
    }
    return StatusResult{ECE141::unknownCommand};
  }
  
  // USE: retrieve active db (if available) to subystem...
  Database* CommandProcessor::getActiveDatabase() {
    return next ? next->getActiveDatabase() : nullptr;
  }

}
