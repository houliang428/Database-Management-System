//
//  BlockVisitor_h
//  Datatabase4

#ifndef BlockVisitor_h
#define BlockVisitor_h

#include "Storage.hpp"
#include "Entity.hpp"
#include "Filters.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  static bool inGivenFields(const std::string &aFieldName, const StringList &aFields) {
    auto theEnd = aFields.end();
    return std::find(aFields.begin(), theEnd, aFieldName) != theEnd;
  }
  
  class BlockVisitor {
  public:
    BlockVisitor(const Entity &anEntity, const Filters &aFilters)
    : entity(anEntity), filters(aFilters) {}
    
    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      if('D'==aBlock.header.type) {
        if(0==aBlock.header.extra) {
        }
      }
      return StatusResult{noError};
    }
    
    const Entity      &entity;
    const Filters     &filters;
  };
  
  class Storage;
  
  class rowRetriever: public BlockVisitor{
  public:
    
    rowRetriever(const Entity &anEntity,const Filters &aFilters,const StringList* aFields, RowCollection &aCollection):
    BlockVisitor(anEntity,aFilters),theFields(aFields), Rows(aCollection) {}
    
    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      if('D'==aBlock.header.type) {
        if(aBlock.header.extra == entity.getHash()){
          Row *theRow = new Row(aBlockNum);
          
          AttributeList theAttrCollection = entity.getAttributes();
          unsigned int index = 0;

          for(auto attr:theAttrCollection){
            std::string key = attr.getName();
            while(index < sizeof(aBlock.data) && aBlock.data[index] != '\0'){
              std::string type;
              Value aValue;
              while(aBlock.data[index] != '^'){
                type += aBlock.data[index++];
              }
              index++;
              std::string dataMem;
              while(aBlock.data[index] != '^'){
                dataMem += aBlock.data[index++];
              }
              index++;
              
              if(type == "I"){
                int data_int = std::stoi(dataMem);
                aValue = data_int;
              }else if(type == "F"){
                float data_float = std::stof(dataMem);
                aValue = data_float;
              }else if(type == "B"){
                bool data_bool = dataMem[0]=='t' ? true:false;
                aValue = data_bool;
              }else if(type == "T"){
                time_t data_time = (time_t)std::stoi(dataMem);
                aValue = data_time;
              }else if(type == "V"){
                aValue = dataMem;
              }
              
              if(!theFields || inGivenFields(attr.getName(),*theFields)){
                theRow->addColumn(key, aValue);
                break;
              }
            }
          }
         
          //rows.push_back(theRow);
          KeyValues test = theRow->getColumns();
          if(filters.matches(test)){
            theRow->setBlockNumber(aBlockNum);
            Rows.add(theRow);
          }
        }
      }
      return StatusResult{noError};
    }
    
    RowCollection     &Rows;
    const StringList  *theFields;
  };
  
  
  class DropTable{
  public:
    
    DropTable(uint32_t nameCode): hashName(nameCode) {}
    
    StatusResult operator()(Storage &aStorage, const Block &aBlock, uint32_t aBlockNum) {
      if('D'==aBlock.header.type) {
        if(aBlock.header.extra == hashName){
          aStorage.releaseBlock(aBlockNum);
        }
      }
      return StatusResult{noError};
    }
    
    uint32_t hashName;
  };
  
#endif /* BlockVisitor_h */

}
