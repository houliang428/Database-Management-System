//
//  Row.hpp
//  Database3
//


#ifndef Row_h
#define Row_h

#include <stdio.h>
#include <vector>
#include "Value.hpp"
#include "Entity.hpp"

namespace ECE141 {
  
  class Block;
  class Entity;
    
  class Row {
    friend class        RowCollection;
    friend class        Database;
    friend class        RecordsView;
    friend class        Filters;
    
  public:
    Row(int32_t aBlockNumber=0);
    Row(const Row &aRow);
    Row(const Block &aBlock); //decode from raw storage...
    Row(KeyValues &aKeyValueList); //build a row
    Row& addColumn(const std::string &aString, const Value &aValue);
    
    ~Row();
    
    int32_t             getBlockNumber() const {return blockNumber;}
    void                setBlockNumber(int32_t aBlockNum) {blockNumber = aBlockNum;}
    const KeyValues&    getColumns() const {return columns;}
    
    bool                operator==(const Row &aCopy);

    friend class  RowCollection;
    
  protected:
    int32_t       blockNumber;
    std::map<std::string, Value> columns;
  };
  
  //-------------------------------------------
  
  using RowList = std::vector<Row*>;
  
  class RowCollection {
  public:
    RowCollection();
    RowCollection(const RowCollection &aCopy);
    ~RowCollection();
    
    RowList&          getRows() {return rows;}
    RowCollection&    add(Row* aRow);
    RowCollection&    reorder(const PropertyList &anOrder, Entity &anEntity);
  protected:
    RowList rows;
  };
  
}
#endif /* Row_h */
