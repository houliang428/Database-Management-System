//
//  SQLInterpreter.hpp
//  Datatabase4
//


#ifndef SQLInterpreter_hpp
#define SQLInterpreter_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Row.hpp"
#include "Attribute.hpp"

namespace ECE141 {
  
  class Entity; //frwd...
  class Filters;
  
  class SQLInterpreter : public CommandProcessor {
  public:
    
    SQLInterpreter(IInterpreter *aNext=nullptr);
    virtual ~SQLInterpreter();
    
    StatusResult          createTable(Entity *anEntity); //low level DB call...
    StatusResult          dropTable(const std::string &aName); //low level DB call...
    StatusResult          showTables();
    StatusResult          alterTable(const std::string &aName,const Attribute &anAttribute,const std::string action);
      
      
    StatusResult          insertRow(const Row &aRow, const std::string &aTableName); //low level DB call...
    StatusResult          deleteRows(const std::string &aTableName, const Filters &aFilters); //low level
    StatusResult          describeTable(const std::string &aName, std::ostream &anOutput);
    StatusResult          selectRows(const std::string &aTableName,
                                     const Filters &aFilters, const PropertyList &anOrder); //low level
    StatusResult          updateRows(const std::string &aName, const KeyValues &aKeyValues, const Filters &aFilters);
  protected:
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
  };

}

#endif /* SQLInterpreter_hpp */
