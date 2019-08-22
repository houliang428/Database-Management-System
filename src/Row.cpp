//
//  Row.cpp
//  ECEDatabase
//


#include "Row.hpp"
#include "Storage.hpp"

namespace ECE141 {
  
  Row::Row(int32_t aBlockNumber) : blockNumber(aBlockNumber), columns() {
  }
  
  Row::Row(const Row &aCopy) : blockNumber(aCopy.blockNumber), columns(aCopy.columns) {
  }
  
  Row::Row(const Block &aBlock) {
    //STUDENT: Implmenent this method...
    KeyValues &newColumns = columns;
    int index = 0;
    while(index < sizeof(aBlock.data) && aBlock.data[index] != '\0'){
      std::string keys;
      while(aBlock.data[index] != '^'){
        keys += aBlock.data[index++];
      }
      index++;
      std::string type;
      while(aBlock.data[index] != '^'){
        type += aBlock.data[index++];
      }
      index++;
      std::string dataMem;
      while(aBlock.data[index] != '^'){
        dataMem += aBlock.data[index++];
      }
      index++;
      
      if(type == "N"){
        Value aValue;
        newColumns[keys] = aValue;
      }
      else if(type == "I"){
        int data_int = std::stoi(dataMem);
        Value aValue = data_int;
        newColumns[keys] = aValue;
      }
      else if(type == "F"){
        float data_float = std::stof(dataMem);
        Value aValue = data_float;
        newColumns[keys] = aValue;
      }
      else if(type == "B"){
        bool data_bool = dataMem[0]=='t' ? true:false;
        Value aValue = data_bool;
        newColumns[keys] = aValue;
      }
      else if(type == "T"){
        time_t data_time = (time_t)std::stoi(dataMem);
        Value aValue = data_time;
        newColumns[keys] = aValue;
      }
      else if(type == "V"){
        Value aValue = dataMem;
        newColumns[keys] = aValue;
      }
      
    }
  }
  Row::Row(KeyValues &aKeyValueList){
    columns = aKeyValueList;
  }
  
  Row& Row::addColumn(const std::string &aString, const Value &aValue){
    columns.insert(std::make_pair(aString,aValue));
    return *this;
  }
  
  Row::~Row() {
    //STUDENT: implement this too
  }
  
  bool Row::operator==(const Row &aCopy) {
    Row row = const_cast<Row&>(aCopy);
    KeyValues column1 = row.getColumns();
    
    if(column1.size() != columns.size())  return false;
    for(auto it1 = columns.begin(); it1 != columns.end(); it1++){
      std::string keys = it1->first;
      if( !(it1->second==column1[keys]) )  return false;
      
    }
    return true; //STUDENT: implement this too
  }

  //---------------------------------------------------
  
  RowCollection::RowCollection() : rows() {}
  
  RowCollection::RowCollection(const RowCollection &aCopy) : rows(aCopy.rows) {}
  
  RowCollection::~RowCollection() {
    for(auto theRow : rows) {
      delete theRow;
    }
  }
  
  RowCollection& RowCollection::add(Row* aRow) {
    rows.push_back(aRow);
    return *this;
  }
  
  RowCollection& RowCollection::reorder(const PropertyList &anOrder, Entity &anEntity){
    
    for(auto property:anOrder){
      if(anEntity.isValidProperty(property)){
        sort( rows.begin(),rows.end(),[property](Row *aLHS, Row* aRHS)
             {return aLHS->columns[property.name] < aRHS->columns[property.name];} );
      }
    }

    return *this;
  }

}


