//
//  Schema.cpp
//  ECEDatabase


#include "Entity.hpp"
#include "Storage.hpp"
#include "Row.hpp"
#include "Tokenizer.hpp"
//#include "BlockVisitor.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <streambuf>
#include <sys/stat.h>
#include "Helpers.hpp"
#include "MemoryStream.hpp"
#include "View.hpp"
#include "Filters.hpp"
#include <ctime>

const int gMultiplier = 37;

namespace ECE141 {
  
  //utility...

  uint32_t Entity::hashString(const char *str) {
    uint32_t h{0};
    unsigned char *p;
    for (p = (unsigned char*)str; *p != '\0'; p++)
      h = gMultiplier * h + *p;
    return h;
  }
 
  //----------------------------------------------------
  
  Entity::Entity(std::string aName)
    : name(aName), attributes(), dirty(false), count(0), blockNum(0), autoincr(0) {
    hash = hashString(aName.c_str());
  }
  
  Entity::Entity(const Entity &aCopy) :
    name(aCopy.name), attributes(aCopy.attributes), dirty(aCopy.dirty), autoincr(aCopy.autoincr),
    count(aCopy.count), blockNum(aCopy.blockNum), hash(aCopy.hash)  {
  }
  
  Entity::Entity(Block &aBlock, uint32_t aBlockNum) {
    //STUDENT: construct an entity (decode) from the given block...
    int index = 0;
    //Entity Name
    std::string nameEn;
    while(aBlock.data[index] != '^'){
      nameEn += aBlock.data[index++];
    }
    name = nameEn;
    index++;
    //Entity count
    std::string countEn;
    while(aBlock.data[index] != '^'){
      countEn += aBlock.data[index++];
    }
    count = (uint16_t)std::stoi(countEn);
    index++;
    //Entity autoincre
    std::string autoincreEn;
    while(aBlock.data[index] != '^'){
      autoincreEn += aBlock.data[index++];
    }
    autoincr = (uint32_t)std::stoi(autoincreEn);
    index++;
    //Entity blockNum
    std::string blockNumEn;
    while(aBlock.data[index] != '^'){
      blockNumEn += aBlock.data[index++];
    }
    blockNum = (uint32_t)std::stoi(blockNumEn);
    index++;
    //Entity hash
    std::string hashEN;
    while(aBlock.data[index] != '^'){
      hashEN += aBlock.data[index++];
    }
    hash = (uint32_t)std::stoi(hashEN);
    index++;
    //Entity dirty
    std::string dirtyEN;
    while(aBlock.data[index] != '#'){
      dirtyEN += aBlock.data[index++];
    }
    dirty = (std::stoi(dirtyEN)==1)?true:false;
    index++;
    
    //attribute decoding:
    while (index<sizeof(aBlock.data) && aBlock.data[index] != '\0') {
      Attribute theAttr;
      
      std::string attrname;
      while(aBlock.data[index] != '^'){
        attrname += aBlock.data[index++];
      }
      theAttr.setName(attrname);
      index++;
      
      std::string attrType;
      while(aBlock.data[index] != '^'){
        attrType += aBlock.data[index++];
      }
      theAttr.setType(MapForDecode[attrType]);
      index++;
      
      std::string attrsize;
      while(aBlock.data[index] != '^'){
        attrsize += aBlock.data[index++];
      }
      theAttr.setSize(std::stoi(attrsize));
      index++;
      
      std::string attrAuto;
      while(aBlock.data[index] != '^'){
        attrAuto += aBlock.data[index++];
      }
      theAttr.setAutoIncrement((bool)std::stoi(attrAuto));
      index++;
      
      std::string attrPRI;
      while(aBlock.data[index] != '^'){
        attrPRI += aBlock.data[index++];
      }
      theAttr.setPrimaryKey((bool)std::stoi(attrPRI));
      index++;
      
      std::string attrNULL;
      while(aBlock.data[index] != '^'){
        attrNULL += aBlock.data[index++];
      }
      theAttr.setNullable((bool)std::stoi(attrNULL));
      index++;
      
      attributes.push_back(theAttr);
    }
    count = attributes.size();
    blockNum = aBlockNum;
  }
    
  Entity::Entity() {}

  // USE: ask the entity for name of primary key (may not have one...)
  std::string Entity::getPrimaryKey() {
    return std::string("id"); //XXX-HACK wrong
  }
  
  // USE: ask entity for next auto-incr value  (for a new record)....
  uint32_t Entity::getNextAutoIncrementValue() {
    dirty=true; //so we resave when DB is released...
    return ++autoincr;
  }

  uint32_t Entity::getHash() const{
    return hash;
  }
  
  void Entity::each(attribute_callback aCallback) const {
    for(auto theAttr : attributes) {
      aCallback(theAttr);
    }
  }


  Entity& Entity::markDirty(bool aState) {
    dirty=aState;
    return *this;
  }

  Entity& Entity::addAttribute(std::string aName, DataType aType, uint8_t aSize, bool autoIncr) {
    attributes.push_back(Attribute(aName, aType, aSize));
    return *this;
  }
  
  Entity& Entity::addAttribute(const Attribute &anAttribute) {
    attributes.push_back(anAttribute);
    return *this;
  }
  
  Attribute& Entity::getAttribute(const std::string &aName)  {
    for(auto theIter = attributes.begin(); theIter != attributes.end(); theIter++) {
      if(theIter->getName()==aName) {
        return *theIter;
      }
    }
    throw std::runtime_error("Attribute not found");
  }
  
  Value Entity::getDefaultValue(const Attribute &anAttribute) {
    Value theResult;
    theResult.become(anAttribute.getType());
    return theResult;
  }
  
  DataType Entity::getPrimaryKeyType(){
    std::string name = getPrimaryKey();
    DataType theType = DataType::no_type;
    for(auto attr:attributes){
      if(attr.getName()==name){
        theType = attr.getType();
        break;
      }
    }
    return theType;
  }

  //Encode (pack) this entity into a storage block...
  Entity::operator Block() {
    Block theBlock{kEntityBlockType};

    //STUDENT: Decode your entity into a  block, and return it...
    theBlock.header.id = blockNum;      //storage Location
    unsigned int index = 0;
    //Entity information
    std::string entity = name+"^"+std::to_string(count)+"^"+std::to_string(autoincr)+"^"+std::to_string(blockNum)+"^"+std::to_string(hash)+"^" + std::to_string(dirty)+"#";
    strcpy(theBlock.data+index, entity.c_str());
    index += entity.size();
    
    //Attributes Information
    for(Attribute attr:attributes){
      std::string attrCode = attr.getName() + "^" + MapforTable[attr.getType()] + "^" + std::to_string(attr.getSize()) + "^" + std::to_string(attr.isAutoIncrement())
                             + "^" + std::to_string(attr.isPrimaryKey()) + "^" + std::to_string(attr.isNullable()) + "^";
      strcpy(theBlock.data+index, attrCode.c_str());
      index += attrCode.size();
    }
    
    return theBlock;
  }

  //** USE:
  StatusResult Entity::validate(const KeyValues &aList) const{
    for (auto theKV : aList) {
      if(!hasCompatibleAttribute(theKV.first, theKV.second)) {
        return StatusResult{invalidAttribute};
      }
    }
    return StatusResult{noError};
  }
  
  bool Entity::hasCompatibleAttribute(std::string key, Value &aValue) const{
    for(auto attr:attributes){
      if(attr.getName()==key && attr.getType()==aValue.getType())
        return true;
    }
    return false;
  }
  
  // USE: write given row into given block (using entity as guide...)
  StatusResult Entity::encodeData(KeyValues &aDataList, Block &aBlock) {
    StatusResult theResult{noError};
    
    // STUDENT: Encode your entity data into the given block...
    unsigned int index;
    for(index =0; index < kPayloadSize; index++){
      if(aBlock.data[index]=='\0')  break;
    }
    std::string encode;
    for(auto attr:attributes){
      if(aDataList.count(attr.getName())){
        std::string key = attr.getName();
        std::string data_string = aDataList[key]; //convert Value to string
        //Encode: field_name^data_type^data_string
        encode = getTypeName[aDataList[key].getType()] + "^" + data_string + "^";
      }
      else{
        std::string encode;
        if(attr.getType() == DataType::int_type)  encode="0";
        else if(attr.getType()==DataType::float_type)  encode = "0.0";
        else if(attr.getType()==DataType::bool_type)   encode = "false";
        else if(attr.getType()==DataType::timestamp_type)   encode = "0";
        else if(attr.getType()==DataType::varchar_type)    encode = "0";
      }
      
      if(index+encode.size()>=kPayloadSize)
        return StatusResult{storageFull};
        
      strcpy(aBlock.data+index, encode.c_str());
      index += (unsigned int) encode.size();
    }
    
    return theResult;
  }

  

  // USE: read data from block into given keyvalue list (for row?)...
  StatusResult Entity::decodeData(const Block &aBlock, KeyValues &aValuesList,
                                  const StringList *aFields) const {
    StatusResult theResult{noError};
    
    unsigned int index = 0;
    unsigned int attr = 0;
    
    while (index<sizeof(aBlock.data) && aBlock.data[index] != '\0') {
      std::string valueType;
      Value theValue;
      DataType theType;
      
      while(aBlock.data[index] != '^'){
        valueType += aBlock.data[index++];
      }
      index++;
      
      std::string value;
      while(aBlock.data[index] != '^'){
        value += aBlock.data[index++];
      }
      index++;
      
      theValue = value;
      if(valueType=="I")      theType = DataType::int_type;
      else if(valueType=="F") theType = DataType::float_type;
      else if(valueType=="V") theType = DataType::varchar_type;
      else if(valueType=="T") theType = DataType::timestamp_type;
      else if(valueType=="B") theType = DataType::bool_type;
      else                    theType = DataType::no_type;
      
      theValue.become(theType);
      Attribute theAttr = attributes[attr++];
      
      if(!aFields ||  find(aFields->begin(),aFields->end(),theAttr.getName())!=aFields->end()){
        aValuesList[theAttr.getName()] = theValue;
      }
    }
    
    
    // STUDENT: Decode your entity from the given block...
    
    return theResult;
  }
  
  StatusResult Entity::isValidProperty(Property &aProperty){
    for(auto attr:attributes){
      if(attr.getName()==aProperty.name)  return StatusResult{noError};
    }
    return StatusResult{invalidArguments};
  }
  

}
