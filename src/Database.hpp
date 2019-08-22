//
//  Database.hpp
//  Database3


#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include "Storage.hpp"
#include "Row.hpp"
#include "Entity.hpp"
#include "Filters.hpp"

namespace ECE141 {
  
  class Statement;

  class Database  {
  public:
    
    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);

    //Database(const Database &aDatabase);
    
    ~Database();
    
    std::string&    getName() {return name;}
    Entity*         getEntity(const std::string &aName);
    Index*          getIndexForTableField(const std::string tablename);
    
    StatusResult    describe(std::ostream &anOutput); //dump the DB (storage) for debug purpose
    StatusResult    saveEntities();
    StatusResult    saveIndexes();
    StatusResult    WriteIndex(Index* theIndex);
    
    
    //Table related...
    
    StatusResult    createTable(Entity *anEntity);
    StatusResult    deleteRows(const Entity &anEntity, const Filters &aFilters);
    StatusResult    dropTable(const std::string &aName);
    StatusResult    alterTable(Entity &anEntity, const Attribute &anAttribute, std::string action);
    
    StatusResult    insertRow(const Row &aRow, const std::string &aTableName);
    
    StatusResult    showTables(std::ostream &anOutput);
    
    StatusResult    selectRows(RowCollection &aColl,    const Entity &anEntity,
                               const Filters &aFilters, const StringList *aList=nullptr);
    
    bool            tableExists(const std::string aName);
    
    StatusResult    updateRow(const Row &aRow, const KeyValues &aKVList, const Entity &anEntity);
    void            DropIndexes();
    
  protected:
    std::string                     name;
    Storage                         storage;
    std::map<std::string, Index*>   PRI_index;
    std::map<std::string, Entity*>  entities;
    std::ofstream    stream;
  };
  
}

#endif /* Database_hpp */
